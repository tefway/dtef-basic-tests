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

using namespace Poco::Net;
using namespace Poco;

static std::atomic<bool> stop = false;
static ClasseIntegracao integ;

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

static std::mutex clientsMtx;
static std::deque<Client> clients;

struct Message {
    std::string message;
    WebSocket ws;
};

static std::queue<Message> messages;
static std::shared_mutex messagesMtx;

static auto findClientNotLock(const WebSocket &ws) {
    auto it =
        std::find_if(clients.begin(), clients.end(),
                     [&ws](const Client &client) { return client.ws == ws; });

    if (it == clients.end()) {
        throw std::runtime_error("Client not found");
    }

    return it;
}

static auto jsonToStr(const Poco::JSON::Object::Ptr json) {
    std::string message;

    {
        std::stringstream sstr;
        json->stringify(sstr);
        message = sstr.str();
    }

    return message;
}

static void broadcastJson(const Poco::JSON::Object::Ptr json) {
    std::string message;

    {
        std::stringstream sstr;
        json->stringify(sstr);
        message = sstr.str();
    }

    std::cout << "Broadcasting message: " << message << std::endl;

    for (auto &client : clients) {
        try {
            std::scoped_lock<std::mutex> lock(client.mtx);
            client.ws.sendFrame(message.c_str(),
                                static_cast<int>(message.size()),
                                WebSocket::FRAME_TEXT);
        } catch (const Poco::Exception &e) {
            std::cerr << "WebSocket error: " << e.displayText() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Error broadcasting message: " << e.what()
                      << std::endl;
        }
    }
}

static auto initMessage(const std::string &type) {
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    obj->set("requestType", type);
    return obj;
}

static auto convertTextToUTF8(std::string_view text) {
    if (text.data() == nullptr) {
        return std::string();
    }

    if (text.empty()) {
        return std::string();
    }

    // Check if the text is valid UTF-8
    Poco::UTF8Encoding utf8;

    bool isValid = true;

    for (auto it = text.begin(); it != text.end(); ++it) {
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

/*
const RequestType = Object.freeze({
    INICIALIZA: 'inicializa',
    FINALIZA: 'finaliza',
    VERSAO: 'versao',
    PROCURA: 'procura',
    TRANSACAO_DEBITO: 'transacaoDebito',
    TRANSACAO_CREDITO: 'transacaoCredito',
    TRANSACAO_VOUCHER: 'transacaoVoucher',
    CONFIRMA: 'confirma',
    CONSULTA_PDV: 'consultaPDV',
    DISPLAY: 'display',
    ERROR: 'error',
    MESSAGE: 'message',
    BEEP: 'beep',
});
*/

static void *CALLING_COV display_error(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("error");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);

    return msg;
}

static void *CALLING_COV display_msg(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("message");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);
    return msg;
}

static void *CALLING_COV display_terminal(char *msg) {
    auto msgUtf8 = convertTextToUTF8(msg);
    std::cout << __func__ << ": " << msgUtf8 << std::endl;
    auto msgToCli = initMessage("terminal");

    msgToCli->set("message", msgUtf8);

    broadcastJson(msgToCli);
    return msg;
}

static void *CALLING_COV callback_beep() {
    std::cout << __func__ << std::endl;
    auto msgToCli = initMessage("beep");

    broadcastJson(msgToCli);
    return nullptr;
}

static std::atomic<bool> sel_op_aguardando = false;

template <class StrType = std::string>
static auto split(std::string_view strview, std::string_view term)
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

static auto parseOpcoes(const std::string &opcoes) {
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

static int callback_seleciona_op(char *pLabel, char *pOpcoes,
                                 int *iOpcaoSelecionada) {
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
            std::cerr << "Error parsing message: " << e.what() << ": "
                      << message.message << std::endl;

            if (!messages.empty()) {
                messages.pop();
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

static void messageCredito(const Poco::JSON::Object::Ptr &obj) {
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

static void messageDebito(const Poco::JSON::Object::Ptr &obj) {
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

static void messageVoucher(const Poco::JSON::Object::Ptr &obj) {
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

static void messageConfirma(const Poco::JSON::Object::Ptr &obj) {
    auto numeroControle = obj->getValue<std::string>("numeroControle");

    auto retn = integ.ConfirmaCartao(numeroControle.data());

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

static void messageDesfaz(const Poco::JSON::Object::Ptr &obj) {
    auto numeroControle = obj->getValue<std::string>("numeroControle");

    auto retn = integ.DesfazCartao(numeroControle.data());

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

static void messageProcuraPinPad(const Poco::JSON::Object::Ptr &obj) {
    char buffer[1024]{};
    auto retn = integ.ProcuraPinPad(buffer);

    auto str = convertTextToUTF8(buffer);

    auto msgToCli = obj;
    msgToCli->set("retn", retn);
    msgToCli->set("dados", str);

    broadcastJson(msgToCli);
}

static void messageVersao(const Poco::JSON::Object::Ptr &obj) {
    char buffer[1024]{};
    integ.VersaoDPOS(buffer);

    auto str = convertTextToUTF8(buffer);

    auto msgToCli = obj;
    msgToCli->set("versao", str);

    broadcastJson(msgToCli);
}

static void messageInicializa(const Poco::JSON::Object::Ptr &obj) {
    int retn = integ.InicializaDPOS();

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

static void messageFinaliza(const Poco::JSON::Object::Ptr &obj) {
    int retn = integ.FinalizaDPOS();

    auto msgToCli = obj;
    msgToCli->set("retn", retn);

    broadcastJson(msgToCli);
}

static void processMessages() {
    std::unique_lock<std::shared_mutex> lock(messagesMtx);
    if (messages.empty()) {
        return;
    }

    auto message = messages.front();

    if (message.message == "stop") {
        stop.store(true);
        return;
    }

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
                     {"finaliza", messageFinaliza}};

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
                } catch (const std::exception &e) {
                    std::cerr << "Error processing message: " << e.what()
                              << std::endl;
                }
            } else {
                std::cerr << "Invalid message type: " << type << std::endl;
                return;
            }
        } else {
            std::cerr << "Invalid message: " << message.message << std::endl;
            auto err = initMessage("error");
            err->set("message", "Invalid message:" + message.message);
            std::scoped_lock<std::mutex> lck(clientsMtx);

            auto cli = findClientNotLock(message.ws);
            std::scoped_lock<std::mutex> lck2(cli->mtx);

            auto msgToCli = jsonToStr(err);
            cli->ws.sendFrame(msgToCli.c_str(),
                              static_cast<int>(msgToCli.size()),
                              WebSocket::FRAME_TEXT);

            messages.pop();
            return;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error broadcasting message: " << e.what() << std::endl;

        if (lock.owns_lock() && !messages.empty()) {
            messages.pop();
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

static void pushMessage(std::string message, WebSocket &ws) {
    std::unique_lock<std::shared_mutex> lock(messagesMtx);
    messages.push({std::move(message), ws});
}

static void broadcastAndClose() {
    for (auto &client : clients) {
        std::scoped_lock<std::mutex> lock(client.mtx);
        try {
            client.ws.sendFrame("", 0, WebSocket::FRAME_OP_CLOSE);
            client.ws.close();
        } catch (const Poco::Exception &e) {
            std::cerr << "WebSocket error: " << e.displayText() << std::endl;
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

            Poco::Buffer<char> data(1024); // Buffer to store received data
            std::cout << "WebSocket connection established" << std::endl;

            while (!stop.load()) {
                data.resize(0);
                int flags = 0;

                auto n = ws.receiveFrame(data,
                                         flags); // Receive data from the client

                if (flags & WebSocket::FRAME_OP_CLOSE) {
                    std::cout << "WebSocket connection closed" << std::endl;
                    rmSocket(ws);
                    ws.close();
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
                ws.close();
            }
        } catch (const Poco::Exception &e) {
            std::cerr << "WebSocket error: " << e.displayText() << std::endl;
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

static void signal_callback_handler(int signum) {
    std::cout << "Caught signal " << signum << std::endl;
    stop.store(true);
}

static auto getenvor(const char *name, const char *def = "") -> std::string {
    auto env = std::getenv(name);
    if (env == nullptr) {
        return def;
    }

    return env;
}

int main() {
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

    integ.carregaDll();
    integ.setCallBackDisplayErro(display_error);
    integ.setCallBackDisplayMensagem(display_msg);
    integ.setCallBackDisplayTerminal(display_terminal);
    integ.setCallBackBeep(callback_beep);
    integ.setCallBackSelecionaOpcao(callback_seleciona_op);

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
            processMessages();
            Thread::sleep(500);
        }
    } catch (Poco::Exception &exc) {
        std::cerr << "Server error: " << exc.displayText() << std::endl;
    }

    return 0;
}
