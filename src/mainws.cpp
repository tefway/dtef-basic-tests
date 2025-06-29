#include "CallOnDtor.hpp"
#include "ClasseIntegracao.h"
#include "ParseLog.hpp"
#include <Poco/BinaryReader.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/TextConverter.h>
#include <Poco/ThreadPool.h>
#include <Poco/UTF8Encoding.h>
#include <Poco/Windows1252Encoding.h>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace Poco::Net;
using namespace Poco;

namespace {
std::atomic<bool> stop = false;
ClasseIntegracao integ;

std::chrono::seconds callbackTimeout = std::chrono::seconds(600);

bool ignorarPerguntaValor = true;

struct Client {
    WebSocket ws;
    std::mutex mtx;

    Client(WebSocket wsIn) : ws(std::move(wsIn)) {}

    Client(Client &&climv) : ws(std::move(climv.ws)) {}

    Client &operator=(Client &&climv) {
        ws = std::move(climv.ws);
        return *this;
    }

    ~Client() {
        try {
            ws.shutdown();
            ws.close();
        } catch (const Poco::Exception &e) {
            std::cerr << "WebSocket error: " << e.displayText() << std::endl;
        }
    }
};

std::mutex clientsMtx;
std::deque<Client> clients;

struct Message {
    std::string message;
    WebSocket ws;
};

std::queue<Message> messages;
std::shared_mutex messagesMtx;

auto findClientNotLock(const WebSocket &ws) {
    auto it =
        std::find_if(clients.begin(), clients.end(),
                     [&ws](const Client &client) { return client.ws == ws; });

    if (it == clients.end()) {
        throw std::runtime_error("Client not found");
    }

    return it;
}

auto jsonToStr(const Poco::JSON::Object::Ptr json) {
    std::string message;

    {
        std::stringstream sstr;
        json->stringify(sstr);
        message = sstr.str();
    }

    return message;
}

void broadcastMessage(const std::string &message) {
    std::scoped_lock<std::mutex> lck(clientsMtx);

    std::vector<WebSocket> toRemove;

    for (auto &client : clients) {
        try {
            std::scoped_lock<std::mutex> lock(client.mtx);
            client.ws.sendFrame(message.c_str(),
                                static_cast<int>(message.size()),
                                WebSocket::FRAME_TEXT);
        } catch (const Poco::Exception &e) {
            std::cerr << __func__ << ": WebSocket error: " << e.displayText()
                      << std::endl;
            try {
                toRemove.emplace_back(client.ws);
            } catch (const Poco::Exception &e) {
                std::cerr << __func__
                          << ": WebSocket error: " << e.displayText()
                          << std::endl;
            } catch (const std::exception &e) {
                std::cerr << __func__ << ": " << e.what() << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error broadcasting message: " << e.what()
                      << std::endl;
        }
    }

    for (auto &ws : toRemove) {
        auto it = std::find_if(
            clients.begin(), clients.end(),
            [&ws](const Client &client) { return client.ws == ws; });

        if (it != clients.end()) {
            std::cout << "Removing client" << std::endl;
            clients.erase(it);
        }
    }
}

void broadcastJson(const Poco::JSON::Object::Ptr json) {
    std::string message;

    {
        std::stringstream sstr;
        json->stringify(sstr);
        message = sstr.str();
    }

    std::cout << "Broadcasting message: " << message << std::endl;

    broadcastMessage(message);
}

namespace {
namespace MessageTypes {

const std::string TRANSACAO_CREDITO = "transacaoCredito";
const std::string TRANSACAO_DEBITO = "transacaoDebito";
const std::string TRANSACAO_VOUCHER = "transacaoVoucher";
const std::string CONFIRMA = "confirma";
const std::string DESFAZ = "desfaz";
const std::string PROCURA = "procura";
const std::string VERSAO = "versao";
const std::string INICIALIZA = "inicializa";
const std::string FINALIZA = "finaliza";
const std::string OBTEM_COMPROVANTE = "obtemComprovante";
const std::string CANCELA_OPERACAO = "cancelaOperacao";
const std::string ERROR = "error";
const std::string MESSAGE = "message";
const std::string TERMINAL = "terminal";
const std::string MENSAGEM_ALERTA = "mensagemAlerta";
const std::string MENSAGEM_ADICIONAL = "mensagemAdicional";
const std::string BEEP = "beep";
const std::string PREVIEW_COMPROVANTE = "previewComprovante";
const std::string SELECIONA_OP = "seleciona_op";
const std::string CANCELAMENTO_PAGAMENTO = "cancelamentoPagamento";
const std::string SOLICITA_CONFIRMACAO = "solicitaConfirmacao";
const std::string ENTRA_VALOR = "entraValor";
const std::string ENTRA_VALOR_ESPECIAL = "entraValorEspecial";

} // namespace MessageTypes

const int TEF_OK = 0;
const int TEF_ERRO = -1;

} // namespace

auto initMessage(const std::string &type) {
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    obj->set("requestType", type);
    return obj;
}

auto convertTextToUTF8(std::string_view text) {
    if (text.data() == nullptr) {
        return std::string();
    }

    if (text.empty()) {
        return std::string();
    }

    // Check if the text is valid UTF-8
    Poco::UTF8Encoding utf8;

    bool isValid = true;

    for (auto it = text.begin(); it != text.end(); /**/) {
        int le =
            utf8.sequenceLength(reinterpret_cast<const unsigned char *>(&*it),
                                static_cast<int>(text.end() - it));

        if (le <= 0) {
            isValid = false;
            break;
        }

        if (!utf8.isLegal(reinterpret_cast<const unsigned char *>(&*it), le)) {
            isValid = false;
        }

        it += le;
    }

    if (isValid) {
        return std::string(text);
    }

    Poco::Windows1252Encoding win1252;

    std::string utf8Text;
    Poco::TextConverter converter(win1252, utf8);
    converter.convert(text.data(), static_cast<int>(text.size()), utf8Text);

    return utf8Text;
}

void *CALLING_COV display_error(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("error");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);

    return msg;
}

void *CALLING_COV display_msg(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("message");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);
    return msg;
}

void *CALLING_COV display_terminal(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("terminal");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);
    return msg;
}

void *CALLING_COV mensagem_alerta(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("mensagemAlerta");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);
    return msg;
}

int CALLING_COV mensagem_adicional(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("mensagemAdicional");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);
    return 0;
}

void *CALLING_COV callback_beep() {
    std::cout << __func__ << std::endl;
    auto msgToCli = initMessage("beep");

    broadcastJson(msgToCli);
    return nullptr;
}

/**
* @brief Comandos genéricos enviados do client Linx TEF. Quando necessário para
executar uma transação, o Paykit poderá chamar este callback para que a
automação execute alguma operação. Uma das situações onde isso é usado é na
apresentação do QR Code para transações com carteiras digitais. Caso os comandos
não sejam implementados, na medida do possível o client do Linx TEF tentará
processar o comando. Por exemplo, caso o comando de apresentação de QR Code não
seja implementado o Paykit exibirá uma tela com o QR Code mesmo que a integração
seja feita com as telas da automação. Campo solicitação 	Formato
Descrição pDadosEntrada 	A 	Dados de solicitação do comando. Os
comandos são enviados no formato TLV, com código de comando com 3 bytes e
tamanho com 6 bytes. O tamanho se refere ao tamanho dos dados e não ao tamanho
total. Por exemplo, o comando para apresentação de um código QR Code fica assim:
001000030LINXeFuiMzDmu85TcYCimGcOeMvUwM
Código de comando: 001
Tamanho: 000030
QRCode: LINXeFuiMzDmu85TcYCimGcOeMvUwM
Campo de Retorno 	Formato 	Descrição
pDadosRetorno 	N 	Dados de retorno da solicitação

Comandos:
Código 	Descrição
001 	Apresentar QR Code. O campo de dados contém o QR Code que deve ser
apresentado 002 	Lista de wallets disponíveis para utilização no QR. Os
nomes são separados por ;

Retorno:
Retorno 	Descrição
0 	O comando foi executado corretamente.
-1 	O comando não foi implementado
-2 	Ocorreu um erro ao executar o comando
*/
int CALLING_COV comandos(char *pDadosEntrada, char *pRetorno) {
    std::cout << __func__ << ": " << (pDadosEntrada ? pDadosEntrada : "null")
              << " " << (pRetorno ? pRetorno : "null") << std::endl;
    /* TLV
    comandos: 002000006003Pix
    comandos:
    00100021600020101021226840014br.gov.bcb.pix2562pix-h.stone.com.br/pix/v2/bf3bf286-2d63-4719-9cbc-4490da6def48520400005303986540510.005802BR5923Nome
    do CPF 071623616846014RIO DE
    JANEIRO622905255c7f6e4e94df7866038269db16304FB46
    */
    if (!pDadosEntrada) {
        std::cerr << __func__ << ": Invalid parameters" << std::endl;
        return -1;
    }

    /*
    pDadosEntrada 	A 	Dados de solicitação do comando.
    Os comandos são enviados no formato TLV, com código de comando com 3 bytes e
    tamanho com 6 bytes. O tamanho se refere ao tamanho dos dados e não ao
    tamanho total. Por exemplo, o comando para apresentação de um código QR Code
    fica assim: 001000030LINXeFuiMzDmu85TcYCimGcOeMvUwM Código de comando: 001
    Tamanho: 000030
    QRCode: LINXeFuiMzDmu85TcYCimGcOeMvUwM
    */
    std::string dadosEntrada(pDadosEntrada);

    if (dadosEntrada.empty()) {
        std::cerr << __func__ << ": Empty command" << std::endl;
        return -1;
    }

    struct TLVCommand {
        std::string tipo;    // Tipo do comando (3 bytes)
        std::string tamanho; // Tamanho dos dados (6 bytes)
        std::string dados;   // Dados do comando
    };

    std::vector<TLVCommand> comandos;

    size_t pos = 0;
    std::string parseError;

    while (pos < dadosEntrada.size()) {
        if (dadosEntrada.size() - pos < 9) {
            std::cerr << __func__ << ": Invalid command format" << std::endl;
            parseError = "Invalid command format";
            break;
        }

        TLVCommand cmd;
        cmd.tipo = dadosEntrada.substr(pos, 3);
        cmd.tamanho = dadosEntrada.substr(pos + 3, 6);
        try {
            size_t tamanho = std::stoul(cmd.tamanho);

            if (dadosEntrada.size() - pos < 9 + tamanho) {
                std::cerr << __func__ << ": Invalid command size" << std::endl;
                parseError = "Invalid command size";
                break;
            }

            cmd.dados = dadosEntrada.substr(pos + 9, tamanho);
            comandos.push_back(cmd);
            pos += 9 + tamanho;
        } catch (const std::exception &e) {
            std::cerr << __func__ << ": Error parsing command: " << e.what()
                      << std::endl;
            parseError = "Error parsing command: " + std::string(e.what()) +
                         " at position " + std::to_string(pos);
            break;
        }
    }

    Poco::JSON::Object::Ptr msgToCli = new Poco::JSON::Object;
    msgToCli->set("requestType", "comandos");
    msgToCli->set("rawData", dadosEntrada);

    Poco::JSON::Array::Ptr arr = new Poco::JSON::Array;
    for (const auto &cmd : comandos) {
        Poco::JSON::Object::Ptr cmdObj = new Poco::JSON::Object;
        cmdObj->set("tipo", cmd.tipo);
        cmdObj->set("tamanho", cmd.tamanho);
        cmdObj->set("dados", cmd.dados);
        arr->add(cmdObj);
    }

    msgToCli->set("comandos", arr);
    msgToCli->set("aguarda_resposta_para_prosseguir", false);

    if (!parseError.empty()) {
        msgToCli->set("error", parseError);
    } else {
        msgToCli->set("error", {});
    }

    broadcastJson(msgToCli);

    return -1; // Manter de momento a resposta como comando não implementado
}

std::atomic<bool> operacaoCancelada = false;

void messageCancelaOperacao(const Poco::JSON::Object::Ptr &obj) {
    operacaoCancelada = true;

    auto msgToCli = obj;
    msgToCli->set("retn", operacaoCancelada.load());

    broadcastJson(msgToCli);
}

void checkOperacaoCancelada() {
    if (stop.load()) {
        operacaoCancelada = true;
        return;
    }

    /*try {
        broadcastMessage("ping");
    } catch (const std::exception &e) {
        std::cerr << "Error pinging operação cancelada: " << e.what()
                  << std::endl;
    }*/

    std::unique_lock<std::shared_mutex> lock(messagesMtx);

    if (messages.empty()) {
        lock.unlock();
        return;
    }

    auto message = messages.front();

    std::cout << "Received message: " << message.message << std::endl;

    if (message.message == "stop") {
        stop.store(true);
        operacaoCancelada = true;
        return;
    }

    try {
        auto obj = Poco::JSON::Parser()
                       .parse(message.message)
                       .extract<Poco::JSON::Object::Ptr>();
        messages.pop();
        lock.unlock();

        if (obj->has("requestType") &&
            obj->getValue<std::string>("requestType") == "cancelaOperacao") {
            messageCancelaOperacao(obj);
            return;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error parsing message: " << e.what() << ": "
                  << message.message << std::endl;
    }
}

int CALLING_COV operacao_cancelada() {
    std::cout << __func__ << " status: " << operacaoCancelada.load()
              << std::endl;

    checkOperacaoCancelada();

    auto resultado = operacaoCancelada.load() ? 1 : 0;
    operacaoCancelada = false;

    return resultado;
}

int CALLING_COV seta_operacao_cancelada(bool bCancelada) {
    std::cout << __func__ << ": " << bCancelada << std::endl;
    operacaoCancelada = bCancelada;
    return 0;
}

std::atomic<bool> sel_op_aguardando = false;

template <class StrType = std::string>
auto split(std::string_view strview, std::string_view term)
    -> std::vector<StrType> {
    size_t current = 0;
    std::vector<StrType> result;

    if (strview.empty()) {
        return result;
    }

    if (term.empty()) {
        result.emplace_back(strview);
        return result;
    }

    do {
        auto sep = strview.find(term, current);

        result.emplace_back(strview.substr(
            current, (sep == std::string_view::npos) ? sep : (sep - current)));

        if (sep == std::string_view::npos) {
            break;
        }

        current = sep + term.size();
    } while (current < strview.size());

    return result;
}

auto parseOpcoes(const std::string &opcoes) {
    /**
     * opcoes (1,"1-A VISTA")#(2,"2-CDC")
     */

    Poco::JSON::Array::Ptr arr = new Poco::JSON::Array;

    auto listaOpcs = split<std::string_view>(opcoes, "#");

    for (auto op : listaOpcs) {
        if (op.size() < 5) {
            continue;
        }

        // skip ( and )
        if (op.front() == '(') {
            op.remove_prefix(1);
        }

        if (op.back() == ')') {
            op.remove_suffix(1);
        }

        auto keyval = split(op, ",");
        if (keyval.size() != 2) {
            std::cerr << "Invalid opcao: " << op << std::endl;
            continue;
        }

        Poco::JSON::Object::Ptr opcao = new Poco::JSON::Object;
        opcao->set("op", std::stoi(keyval[0]));
        opcao->set("desc", std::string(keyval[1]));

        arr->add(opcao);
    }

    return arr;
}

void *CALLING_COV callBackPreviewComprovante(char *pComprovante) {
    auto comprovante = convertTextToUTF8(pComprovante);

    auto msgToCli = initMessage("previewComprovante");
    msgToCli->set("comprovante", comprovante);

    broadcastJson(msgToCli);
    return nullptr;
}

int CALLING_COV callbackSolicitaConfirmacao(char *pMensagem) {
    auto msgTxt = convertTextToUTF8(pMensagem);
    std::cout << __func__ << ": " << msgTxt << std::endl;

    auto msg = initMessage(MessageTypes::SOLICITA_CONFIRMACAO);

    if (msgTxt.empty()) {
        msgTxt = "Confirma?";
    }

    msg->set("mensagem", msgTxt);

    broadcastJson(msg);

    int result = 0;

    while (!stop) {
        std::unique_lock<std::shared_mutex> lock(messagesMtx);

        if (messages.empty()) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto message = messages.front();

        std::cout << "Received message: " << message.message << std::endl;

        if (message.message == "stop") {
            stop.store(true);
            return 1;
        }

        try {
            auto obj = Poco::JSON::Parser()
                           .parse(message.message)
                           .extract<Poco::JSON::Object::Ptr>();
            messages.pop();

            if (obj->has("requestType") &&
                obj->getValue<std::string>("requestType") ==
                    MessageTypes::SOLICITA_CONFIRMACAO) {
                auto op = obj->getValue<int>("op");

                result = op;
                std::cout << __func__ << ": " << result << std::endl;
                return op;
            }

            std::cout << __func__ << ": "
                      << "Invalid message: " << message.message << std::endl;

            std::scoped_lock<std::mutex> lck(clientsMtx);

            auto cli = findClientNotLock(message.ws);
            std::scoped_lock<std::mutex> lck2(cli->mtx);

            auto msgToCli = jsonToStr(msg);
            cli->ws.sendFrame(msgToCli.c_str(),
                              static_cast<int>(msgToCli.size()),
                              WebSocket::FRAME_TEXT);
        } catch (const std::exception &e) {
            try {
                std::cerr << "Error parsing message: " << e.what() << ": "
                          << message.message << std::endl;
                std::scoped_lock<std::mutex> lck(clientsMtx);

                auto cli = findClientNotLock(message.ws);
                std::scoped_lock<std::mutex> lck2(cli->mtx);

                auto msgToCli = jsonToStr(msg);
                cli->ws.sendFrame(msgToCli.c_str(),
                                  static_cast<int>(msgToCli.size()),
                                  WebSocket::FRAME_TEXT);
            } catch (const std::exception &e) {
                std::cerr << "Error parsing message: " << e.what() << ": "
                          << message.message << std::endl;
            }

            if (!messages.empty()) {
                messages.pop();
            }
            lock.unlock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return result;
}

int aguardaResposta(Poco::JSON::Object::Ptr msg, const std::string &type,
                    const std::function<int(Poco::JSON::Object::Ptr)> &cbFN) {
    auto start = std::chrono::steady_clock::now();

    while (!stop) {
        if (std::chrono::steady_clock::now() - start > callbackTimeout) {
            std::cerr << "Timeout waiting for response" << std::endl;
            return TEF_OK;
        }

        std::unique_lock<std::shared_mutex> lock(messagesMtx);

        if (messages.empty()) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto message = messages.front();

        std::cout << "Received message: " << message.message << std::endl;

        if (message.message == "stop") {
            stop.store(true);
            return 1;
        }

        try {
            auto obj = Poco::JSON::Parser()
                           .parse(message.message)
                           .extract<Poco::JSON::Object::Ptr>();
            messages.pop();

            if (obj->has("requestType") &&
                obj->getValue<std::string>("requestType") == type) {
                return cbFN(obj);
            }

            std::cout << __func__ << ": "
                      << "Invalid message: " << message.message << std::endl;

            std::scoped_lock<std::mutex> lck(clientsMtx);

            auto cli = findClientNotLock(message.ws);
            std::scoped_lock<std::mutex> lck2(cli->mtx);

            auto msgToCli = jsonToStr(msg);
            cli->ws.sendFrame(msgToCli.c_str(),
                              static_cast<int>(msgToCli.size()),
                              WebSocket::FRAME_TEXT);
        } catch (const std::exception &e) {
            try {
                std::cerr << "Error parsing message: " << e.what() << ": "
                          << message.message << std::endl;
                std::scoped_lock<std::mutex> lck(clientsMtx);

                auto cli = findClientNotLock(message.ws);
                std::scoped_lock<std::mutex> lck2(cli->mtx);

                auto msgToCli = jsonToStr(msg);
                cli->ws.sendFrame(msgToCli.c_str(),
                                  static_cast<int>(msgToCli.size()),
                                  WebSocket::FRAME_TEXT);
            } catch (const std::exception &e) {
                std::cerr << "Error parsing message: " << e.what() << ": "
                          << message.message << std::endl;
            }

            if (!messages.empty()) {
                messages.pop();
            }
            lock.unlock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return TEF_OK;
}

int CALLING_COV callbackEntraCartao(char *pLabel, char *pCartao) {
    auto label = convertTextToUTF8(pLabel);
    auto cartao = convertTextToUTF8(pCartao);
    std::cout << __func__ << ": " << label << std::endl;
    std::cout << __func__ << ": " << cartao << std::endl;

    auto msg = initMessage("entraCartao");
    msg->set("label", label);
    msg->set("cartao", cartao);

    broadcastJson(msg);

    return stop.load() ? TEF_ERRO : TEF_OK;
}

int callback_seleciona_op(char *pLabel, char *pOpcoes, int *iOpcaoSelecionada) {
    auto label = convertTextToUTF8(pLabel);
    auto opcoes = convertTextToUTF8(pOpcoes);
    std::cout << __func__ << ": " << label << std::endl;
    std::cout << __func__ << ": " << opcoes << std::endl;
    std::cout << __func__ << ": " << *iOpcaoSelecionada << std::endl;

    sel_op_aguardando = true;

    CallOnDtor cod([&]() { sel_op_aguardando = false; });

    auto msg = initMessage(MessageTypes::SELECIONA_OP);
    msg->set("label", label);
    msg->set("opcoes_raw", opcoes);
    try {
        msg->set("opcoes", parseOpcoes(opcoes));
    } catch (const std::exception &e) {
        std::cerr << "Error parsing opcoes: " << e.what() << " " << opcoes
                  << std::endl;
    }

    broadcastJson(msg);

    return aguardaResposta(
        msg, MessageTypes::SELECIONA_OP, [&](Poco::JSON::Object::Ptr obj) {
            auto op = obj->getValue<int>("op");
            auto retn = obj->getNullableValue<int>("retn").value(0);

            *iOpcaoSelecionada = op;
            std::cout << __func__ << ": " << *iOpcaoSelecionada << std::endl;

            return retn;
        });
}

int CALLING_COV callbackEntraValor(char *pLabel, char *pValor,
                                   char *pValorMinimo, char *pValorMaximo) {
    auto label = convertTextToUTF8(pLabel);

    std::cout << __func__ << ": " << label << std::endl;

    auto msg = initMessage(MessageTypes::ENTRA_VALOR);

    msg->set("label", label);

    if (pValor != nullptr) {
        auto valor = convertTextToUTF8(pValor);
        msg->set("valor", valor);

        if (ignorarPerguntaValor && valor.size() > 0) {
            return TEF_OK;
        }
    }

    if (pValorMinimo != nullptr) {
        msg->set("valorMinimo", convertTextToUTF8(pValorMinimo));
    }

    if (pValorMaximo != nullptr) {
        msg->set("valorMaximo", convertTextToUTF8(pValorMaximo));
    }

    broadcastJson(msg);

    aguardaResposta(msg, MessageTypes::ENTRA_VALOR,
                    [&](Poco::JSON::Object::Ptr obj) {
                        auto op = obj->getValue<std::string>("op");

                        if (op == "cancelaOperacao") {
                            return TEF_ERRO;
                        }

                        if (op.empty()) {
                            return TEF_OK;
                        }

                        strcpy(pValor, op.c_str());
                        return TEF_OK;
                    });

    return stop.load() ? TEF_ERRO : TEF_OK;
}

int CALLING_COV callbackEntraValorEspecial(char *pLabel, char *pValor,
                                           char *pParametros) {
    auto label = convertTextToUTF8(pLabel);

    std::cout << __func__ << ": " << label << std::endl;

    auto msg = initMessage(MessageTypes::ENTRA_VALOR_ESPECIAL);

    msg->set("label", label);

    if (pValor != nullptr) {
        auto valor = convertTextToUTF8(pValor);
        msg->set("valor", valor);

        if (ignorarPerguntaValor && valor.size() > 0) {
            return TEF_OK;
        }
    }

    if (pParametros != nullptr) {
        msg->set("parametros", convertTextToUTF8(pParametros));
    }

    broadcastJson(msg);

    aguardaResposta(msg, MessageTypes::ENTRA_VALOR_ESPECIAL,
                    [&](Poco::JSON::Object::Ptr obj) {
                        auto op = obj->getValue<std::string>("op");

                        if (op == "cancelaOperacao") {
                            return TEF_ERRO;
                        }

                        if (op.empty()) {
                            return TEF_OK;
                        }

                        strcpy(pValor, op.c_str());
                        return TEF_OK;
                    });

    return stop.load() ? TEF_ERRO : TEF_OK;
}

void messageCredito(const Poco::JSON::Object::Ptr &obj) {
    auto valor = obj->getValue<std::string>("valor");
    auto cupom = obj->getValue<std::string>("cupom");

    char buffControle[128]{};

    auto retn = integ.TransacaoCartaoCredito(valor, cupom, buffControle);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("numeroControle", std::string(buffControle));

    broadcastJson(msgToCli);
}

void messageDebito(const Poco::JSON::Object::Ptr &obj) {
    auto valor = obj->getValue<std::string>("valor");
    auto cupom = obj->getValue<std::string>("cupom");

    char buffControle[128]{};

    auto retn = integ.TransacaoCartaoDebito(valor, cupom, buffControle);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("numeroControle", std::string(buffControle));

    broadcastJson(msgToCli);
}

void messageVoucher(const Poco::JSON::Object::Ptr &obj) {
    auto valor = obj->getValue<std::string>("valor");
    auto cupom = obj->getValue<std::string>("cupom");

    char buffControle[128]{};

    auto retn = integ.TransacaoCartaoVoucher(valor, cupom, buffControle);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("numeroControle", std::string(buffControle));

    broadcastJson(msgToCli);
}

void messageQrCode(const Poco::JSON::Object::Ptr &obj) {
    auto valor = obj->getValue<std::string>("valor");
    auto cupom = obj->getValue<std::string>("cupom");

    char buffControle[128]{};

    std::string TransactionParamsData;
    try {
        auto TransactionParamsDataObj = obj->getObject("TransactionParamsData");
        if (!TransactionParamsDataObj.isNull()) {
            std::stringstream sstr;
            TransactionParamsDataObj->stringify(sstr);
            TransactionParamsData = sstr.str();
        }
    } catch (const Poco::Exception &e) {
        std::cerr << "Error stringify TransactionParamsData: "
                  << e.displayText() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error stringify TransactionParamsData: " << e.what()
                  << std::endl;
    }

    auto retn = integ.TransacaoQRCode(valor, cupom, buffControle,
                                      TransactionParamsData.data());

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("numeroControle", std::string(buffControle));

    broadcastJson(msgToCli);
}

void messageConfirma(const Poco::JSON::Object::Ptr &obj) {
    auto numeroControle = obj->getValue<std::string>("numeroControle");

    auto retn = integ.ConfirmaCartao(numeroControle.data());

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

void messageCancelamentoPagamento(const Poco::JSON::Object::Ptr &obj) {
    auto numeroControle =
        obj->getNullableValue<std::string>("numeroControle").value("");

    char buffControle[128]{};

    auto retn = integ.TransacaoCancelamentoPagamento(buffControle);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("numeroControle", std::string(buffControle));

    broadcastJson(msgToCli);
}

void messageDesfaz(const Poco::JSON::Object::Ptr &obj) {
    auto numeroControle = obj->getValue<std::string>("numeroControle");

    auto retn = integ.DesfazCartao(numeroControle.data());

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

auto logTransacaoDeprecadaParaJson(const std::string &str)
    -> Poco::JSON::Object::Ptr {
    /*
    Registro do LOG Estendido
    Descrição 	Tamanho 	Observação
    NumeroEmpresa 	4 N
    NumeroLoja 	6 N
    TipoRegistro 	3 A

        [XXX] Mesmo registro da transação original

    Data 	8 N
    Hora 	6 N
    CodigoTransacao 	2 N 	Codigo da transação solicitada
    NumeroEquipamento 	3 N
    NumeroControle 	6 N 	NSU (número sequencial único)
    ValorOriginal 	12 N 	Valor original da transação
    ValorDesconto 	12 N 	Valor do desconto
    ValorFinal 	12 N 	Valor final da transação (Original – Desconto)
    CodigoBandeira 	4 N 	Código da bandeira do cartão (Tabela 5.0)
    CodigoProdutoRede 	4 N 	Código do produto da rede utilizado na
    transação. Será retornado somente para as redes que o utilizarem e “0000” em
    caso contrário. CodigoBandeiraRede 	4 N 	Código da bandeira na rede
    NumeroControleRede 	15 A 	Número do Controle da Rede com 15 posições (no
    lognormal, está com 9) CódigoTEFTerceiro 	2 N 	Código do TEF a que se
    referem os campos de bandeira e rede de terceiro (1=SiTef)
    CodigoRedeTerceiro 	8 N 	Código da Rede do TEF de terceiro
    CodigoBandeiraTerceiro 	8 N 	Código da Bandeira do TEF de terceiro
    CNPJRedeAdquirente 	14 N 	CNPJ da rede adquirente
    NumeroControleRedeAdicional 	32 A 	NSU Rede para rede Stone
    CodigoBandeiraSefaz 	4 N 	Código da Bandeira na Sefaz
    NomePortador 	32 A 	Nome do portador do cartão
    Reservado 	54 A
    CR+LF 	2 H 	[0D0A]

    */

    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;

    obj->set("NumeroEmpresa", str.substr(0, 4));
    obj->set("NumeroLoja", str.substr(4, 6));
    obj->set("TipoRegistro", str.substr(10, 3));
    obj->set("Data", str.substr(13, 8));
    obj->set("Hora", str.substr(21, 6));
    obj->set("CodigoTransacao", str.substr(27, 2));
    obj->set("NumeroEquipamento", str.substr(29, 3));
    obj->set("NumeroControle", str.substr(32, 6));
    obj->set("ValorOriginal", str.substr(38, 12));
    obj->set("ValorDesconto", str.substr(50, 12));
    obj->set("ValorFinal", str.substr(62, 12));
    obj->set("CodigoBandeira", str.substr(74, 4));
    obj->set("CodigoProdutoRede", str.substr(78, 4));
    obj->set("CodigoBandeiraRede", str.substr(82, 4));
    obj->set("NumeroControleRede", str.substr(86, 15));
    obj->set("CodigoTEFTerceiro", str.substr(101, 2));
    obj->set("CodigoRedeTerceiro", str.substr(103, 8));
    obj->set("CodigoBandeiraTerceiro", str.substr(111, 8));
    obj->set("CNPJRedeAdquirente", str.substr(119, 14));
    obj->set("NumeroControleRedeAdicional", str.substr(133, 32));
    obj->set("CodigoBandeiraSefaz", str.substr(165, 4));
    obj->set("NomePortador", str.substr(169, 32));
    obj->set("Reservado", str.substr(201, 54));
    obj->set("CRLF", str.substr(255, 2));

    return obj;
}

void messageObtemLogUltimaTransacao(const Poco::JSON::Object::Ptr &obj) {
    std::array<char, 1024> buffer{};

    integ.ObtemLogUltimaTransacao(buffer.data(), true);
    Poco::Dynamic::Var res;

    std::string str;

    auto msgToCli = obj;

    try {
        str = convertTextToUTF8(buffer.data());
    } catch (const std::exception &e) {
        std::cerr << __func__ << ": " << e.what() << std::endl;
    }

    try {
        std::cout << __func__ << ": " << str << std::endl;

        res = Poco::JSON::Parser().parse(str);
    } catch (const std::exception &e) {
        std::cerr << __func__ << ": " << e.what() << std::endl;
        try {
            msgToCli->set("dados_raw", str);
            auto objtmp = logTransacaoDeprecadaParaJson(str);

            integ.ObtemLogUltimaTransacao(buffer.data(), false);
            try {
                str = convertTextToUTF8(buffer.data());
            } catch (const std::exception &e) {
                std::cerr << __func__ << ": " << e.what() << std::endl;
            }
            msgToCli->set("dados_raw_1", str);

            try {
                parseTransacaoParaJson(convertTextToUTF8(buffer.data()),
                                       objtmp);
            } catch (const Poco::Exception &e) {
                std::cerr << __func__ << ": " << e.what() << std::endl;
                msgToCli->set("dados_raw_err", std::string(e.what()));
            } catch (const std::exception &e) {
                std::cerr << __func__ << ": " << e.what() << std::endl;
                msgToCli->set("dados_raw_err", std::string(e.what()));
            }

            res = objtmp;
        } catch (const std::exception &e) {
            std::cerr << __func__ << ": " << e.what() << std::endl;
        }
    }

    msgToCli->set("dados", res);
    msgToCli->set("retn", res.isEmpty() ? TEF_ERRO : TEF_OK);

    broadcastJson(msgToCli);
}

void messageProcuraPinPad(const Poco::JSON::Object::Ptr &obj) {
    char buffer[1024]{};
    auto retn = integ.ProcuraPinPad(buffer);

    auto str = convertTextToUTF8(buffer);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("dados", str);

    broadcastJson(msgToCli);
}

void messageVersao(const Poco::JSON::Object::Ptr &obj) {
    char buffer[1024]{};
    integ.VersaoDPOS(buffer);

    auto str = convertTextToUTF8(buffer);

    auto msgToCli = obj;
    msgToCli->set("versao", str);

    broadcastJson(msgToCli);
}

void messageInicializa(const Poco::JSON::Object::Ptr &obj) {
    int retn = integ.InicializaDPOS();

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

void messageFinaliza(const Poco::JSON::Object::Ptr &obj) {
    int retn = integ.FinalizaDPOS();

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

void messageFinalizaTransacao(const Poco::JSON::Object::Ptr &obj) {
    int retn = integ.FinalizaTransacao();

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

void messageObtemComprovante(const Poco::JSON::Object::Ptr &obj) {
    auto numeroControle = obj->getValue<std::string>("numeroControle");
    char comprovanteBuffer[1024]{};
    char comprovanteReduzido[1024]{};

    integ.ObtemComprovanteTransacao(numeroControle.data(), comprovanteBuffer,
                                    comprovanteReduzido);

    auto comprovante = convertTextToUTF8(comprovanteBuffer);
    auto comprovanteRed = convertTextToUTF8(comprovanteReduzido);

    auto msgToCli = obj;
    msgToCli->set("retn", 0);
    msgToCli->set("comprovante", comprovante);
    msgToCli->set("comprovanteReduzido", comprovanteRed);

    broadcastJson(msgToCli);
}

void processMessages() {
    std::unique_lock<std::shared_mutex> lock(messagesMtx);
    if (messages.empty()) {
        return;
    }

    auto message = messages.front();

    if (message.message == "stop") {
        stop.store(true);
        return;
    }

    auto alertError = [&](const std::string &error =
                              "Error processing message") {
        try {
            std::cerr << "Invalid message: " << message.message << std::endl;
            auto err = initMessage("error");
            err->set("message", error + ": " + message.message);
            std::scoped_lock<std::mutex> lck(clientsMtx);

            auto cli = findClientNotLock(message.ws);
            std::scoped_lock<std::mutex> lck2(cli->mtx);

            auto msgToCli = jsonToStr(err);
            cli->ws.sendFrame(msgToCli.c_str(),
                              static_cast<int>(msgToCli.size()),
                              WebSocket::FRAME_TEXT);
        } catch (const Poco::Exception &e) {
            std::cerr << "Error processing message: " << e.displayText()
                      << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error processing message: " << e.what() << std::endl;
        }
    };

    std::unordered_map<std::string,
                       std::function<void(const Poco::JSON::Object::Ptr &obj)>>
        messageTypes{
            {"transacaoCredito", messageCredito},
            {"transacaoDebito", messageDebito},
            {"transacaoVoucher", messageVoucher},
            {"transacaoQrCode", messageQrCode},
            {"confirma", messageConfirma},
            {"desfaz", messageDesfaz},
            {"procura", messageProcuraPinPad},
            {"versao", messageVersao},
            {"inicializa", messageInicializa},
            {"finaliza", messageFinaliza},
            {"finalizaTransacao", messageFinalizaTransacao},
            {"obtemComprovante", messageObtemComprovante},
            {"cancelaOperacao", messageCancelaOperacao},
            {"cancelamentoPagamento", messageCancelamentoPagamento},
            {"obtemLogUltimaTransacao", messageObtemLogUltimaTransacao}};

    try {
        auto obj = Poco::JSON::Parser()
                       .parse(message.message)
                       .extract<Poco::JSON::Object::Ptr>();
        if (obj->has("requestType")) {
            auto type = obj->getValue<std::string>("requestType");

            if (messageTypes.find(type) != messageTypes.end()) {
                messages.pop();
                lock.unlock();

                try {
                    messageTypes[type](obj);
                } catch (const Poco::Exception &e) {
                    std::cerr << "Error processing message: " << e.displayText()
                              << std::endl;
                    alertError(e.displayText());
                } catch (const std::exception &e) {
                    std::cerr << "Error processing message: " << e.what()
                              << std::endl;
                    alertError(e.what());
                }
            } else {
                std::cerr << "Invalid message type: " << type << " ";
                obj->stringify(std::cerr);
                std::cerr << std::endl;

                messages.pop();
                return;
            }
        } else {
            alertError();
            messages.pop();
            return;
        }
    } catch (const std::exception &e) {
        std::cerr << __func__ << ": Error broadcasting message: " << e.what()
                  << std::endl;

        if (lock.owns_lock() && !messages.empty()) {
            messages.pop();
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void pushMessage(std::string message, WebSocket &ws) {
    std::unique_lock<std::shared_mutex> lock(messagesMtx);
    messages.push({std::move(message), ws});
}

void broadcastAndClose() {
    std::scoped_lock<std::mutex> lck(clientsMtx);
    for (auto &client : clients) {
        std::scoped_lock<std::mutex> lock(client.mtx);
        try {
            client.ws.sendFrame("", 0, WebSocket::FRAME_OP_CLOSE);
            client.ws.close();
        } catch (const Poco::Exception &e) {
            std::cerr << __func__ << ": WebSocket error: " << e.displayText()
                      << std::endl;
        }
    }
}

class WebSocketRequestHandler : public HTTPRequestHandler {
  public:
    auto pushSocket(WebSocket &ws) -> Client & {
        std::scoped_lock<std::mutex> lck(clientsMtx);
        return clients.emplace_back(ws);
    }

    void rmSocket(WebSocket &ws) {
        std::scoped_lock<std::mutex> lck(clientsMtx);
        auto it = std::find_if(
            clients.begin(), clients.end(),
            [&ws](const Client &client) { return client.ws == ws; });

        if (it != clients.end()) {
            clients.erase(it);
        }
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response) override {
        try {
            WebSocket ws(request, response); // Establish WebSocket connection

            pushSocket(ws);

            CallOnDtor cod([&]() {
                rmSocket(ws);

                try {
                    ws.close();
                } catch (const Poco::Exception &e) {
                    std::cerr << "WebSocket error: " << e.displayText()
                              << std::endl;
                }
            });

            Poco::Buffer<char> data(1024); // Buffer to store received data
            std::cout << "WebSocket connection established" << std::endl;

            while (!stop.load()) {
                data.resize(0);
                int flags = 0;

                auto n = ws.receiveFrame(data,
                                         flags); // Receive data from the client

                if (flags & WebSocket::FRAME_OP_CLOSE) {
                    std::cout << "WebSocket connection closed" << std::endl;
                    break;
                }

                if (strncmp(data.begin(), "ping", data.size()) == 0) {
                    ws.sendFrame("pong", 4, WebSocket::FRAME_TEXT);
                    continue;
                }

                if (strncmp(data.begin(), "pong", data.size()) == 0) {
                    continue;
                }

                if (n > 0) {
                    std::string message(data.begin(), data.size());
                    std::cout << "Received message: " << message << std::endl;
                    pushMessage(message, ws);
                }
            }

            if (stop) {
                ws.sendFrame("", 0, WebSocket::FRAME_OP_CLOSE);
            }
        } catch (const Poco::Exception &e) {
            std::cerr << __func__ << ": WebSocket error: " << e.displayText()
                      << std::endl;
        }
    }

    ~WebSocketRequestHandler() override;
};

WebSocketRequestHandler::~WebSocketRequestHandler() = default;

class WebSocketRequestHandlerFactory : public HTTPRequestHandlerFactory {
  public:
    HTTPRequestHandler *
    createRequestHandler(const HTTPServerRequest &request) override;

    ~WebSocketRequestHandlerFactory() override;
};

WebSocketRequestHandlerFactory::~WebSocketRequestHandlerFactory() = default;

// Provide an out-of-line definition for the createRequestHandler method
HTTPRequestHandler *WebSocketRequestHandlerFactory::createRequestHandler(
    const HTTPServerRequest &request) {
    if (request.find("Upgrade") != request.end() &&
        request["Upgrade"] == "websocket") {
        return new WebSocketRequestHandler; // Handle only WebSocket upgrade
                                            // requests
    } else {
        return nullptr; // Ignore non-WebSocket requests
    }
}

void signal_callback_handler(int signum) {
    std::cout << "Caught signal " << signum << std::endl;
    stop.store(true);
}

auto getenvor(const char *name, const char *def = "") -> std::string {
    auto env = std::getenv(name);
    if (env == nullptr) {
        return def;
    }

    return env;
}

} // namespace

int main() {
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

    integ.carregaDll(getenvor("DPOS_SO_FULL_PATH", "./libDPOSDRV.so").c_str());
    integ.setCallBackDisplayErro(display_error);
    integ.setCallBackDisplayMensagem(display_msg);
    integ.setCallBackDisplayTerminal(display_terminal);
    integ.setCallBackBeep(callback_beep);
    integ.setCallBackSelecionaOpcao(callback_seleciona_op);
    integ.setCallBackMensagemAlerta(mensagem_alerta);
    integ.setCallBackMensagemAdicional(mensagem_adicional);
    integ.setCallBackPreviewComprovante(callBackPreviewComprovante);
    integ.setCallBackOperacaoCancelada(operacao_cancelada);
    integ.setCallBackSetaOperacaoCancelada(seta_operacao_cancelada);
    integ.setCallBackSolicitaConfirmacao(callbackSolicitaConfirmacao);
    integ.setCallBackEntraValor(callbackEntraValor);
    integ.setCallBackEntraValorEspecial(callbackEntraValorEspecial);
    integ.setCallbackComandos(comandos);

    uint16_t port = static_cast<uint16_t>(std::stoul(getenvor("PORT", "9000")));

    try {
        // Set up HTTP server to handle WebSocket requests
        ServerSocket svs(port);
        HTTPServer server(new WebSocketRequestHandlerFactory, svs,
                          new HTTPServerParams);

        server.start(); // Start the server
        std::cout << "WebSocket server started on port " << port << std::endl;

        // Keep the server running
        while (!stop.load()) {
            operacaoCancelada = false;
            processMessages();
            Thread::sleep(500);
        }

        broadcastAndClose();
        server.stopAll();
    } catch (Poco::Exception &exc) {
        std::cerr << "Server error: " << exc.displayText() << std::endl;
    }

    return 0;
}
