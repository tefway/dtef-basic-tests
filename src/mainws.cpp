#include "CallOnDtor.hpp"
#include "ClasseIntegracao.h"
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
#include <csignal>
#include <cstdint>
#include <cstdlib>
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

void broadcastJson(const Poco::JSON::Object::Ptr json) {
    std::string message;

    {
        std::stringstream sstr;
        json->stringify(sstr);
        message = sstr.str();
    }

    std::cout << "Broadcasting message: " << message << std::endl;

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

} // namespace MessageTypes
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
    std::cout << __func__ << std::endl;

    checkOperacaoCancelada();

    return operacaoCancelada.load() ? 1 : 0;
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

int callback_seleciona_op(char *pLabel, char *pOpcoes, int *iOpcaoSelecionada) {
    auto label = convertTextToUTF8(pLabel);
    auto opcoes = convertTextToUTF8(pOpcoes);
    std::cout << __func__ << ": " << label << std::endl;
    std::cout << __func__ << ": " << opcoes << std::endl;
    std::cout << __func__ << ": " << *iOpcaoSelecionada << std::endl;

    sel_op_aguardando = true;

    CallOnDtor cod([&]() { sel_op_aguardando = false; });

    auto msg = initMessage("seleciona_op");
    msg->set("label", label);
    msg->set("opcoes_raw", opcoes);
    try {
        msg->set("opcoes", parseOpcoes(opcoes));
    } catch (const std::exception &e) {
        std::cerr << "Error parsing opcoes: " << e.what() << " " << opcoes
                  << std::endl;
    }

    broadcastJson(msg);

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
                obj->getValue<std::string>("requestType") == "seleciona_op") {
                auto op = obj->getValue<int>("op");
                auto retn = obj->getNullableValue<int>("retn").value(0);

                *iOpcaoSelecionada = op;
                std::cout << __func__ << ": " << *iOpcaoSelecionada
                          << std::endl;
                return retn;
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

    return 0;
}

void messageCredito(const Poco::JSON::Object::Ptr &obj) {
    auto valor = obj->getValue<std::string>("valor");
    auto cupom = obj->getValue<std::string>("cupom");

    char buffControle[128]{};

    auto retn =
        integ.TransacaoCartaoCredito(valor.data(), cupom.data(), buffControle);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("numeroControle", std::string(buffControle));

    broadcastJson(msgToCli);
}

void messageDebito(const Poco::JSON::Object::Ptr &obj) {
    auto valor = obj->getValue<std::string>("valor");
    auto cupom = obj->getValue<std::string>("cupom");

    char buffControle[128]{};

    auto retn =
        integ.TransacaoCartaoDebito(valor.data(), cupom.data(), buffControle);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("numeroControle", std::string(buffControle));

    broadcastJson(msgToCli);
}

void messageVoucher(const Poco::JSON::Object::Ptr &obj) {
    auto valor = obj->getValue<std::string>("valor");
    auto cupom = obj->getValue<std::string>("cupom");

    char buffControle[128]{};

    auto retn =
        integ.TransacaoCartaoVoucher(valor.data(), cupom.data(), buffControle);

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
    auto numeroControle = obj->getValue<std::string>("numeroControle");

    auto retn = integ.TransacaoCancelamentoPagamento(numeroControle.data());

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

void messageDesfaz(const Poco::JSON::Object::Ptr &obj) {
    auto numeroControle = obj->getValue<std::string>("numeroControle");

    auto retn = integ.DesfazCartao(numeroControle.data());

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

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
        messageTypes{{"transacaoCredito", messageCredito},
                     {"transacaoDebito", messageDebito},
                     {"transacaoVoucher", messageVoucher},
                     {"confirma", messageConfirma},
                     {"desfaz", messageDesfaz},
                     {"procura", messageProcuraPinPad},
                     {"versao", messageVersao},
                     {"inicializa", messageInicializa},
                     {"finaliza", messageFinaliza},
                     {"obtemComprovante", messageObtemComprovante},
                     {"cancelaOperacao", messageCancelaOperacao},
                     {"cancelamentoPagamento", messageCancelamentoPagamento}};

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
                std::cerr << "Invalid message type: " << type << std::endl;
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

    integ.carregaDll();
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
