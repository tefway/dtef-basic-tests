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
#include <deque>
#include <iostream>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <sstream>
#include <thread>

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

static auto findClient(const WebSocket &ws) {
    std::scoped_lock<std::mutex> lck(clientsMtx);
    auto it =
        std::find_if(clients.begin(), clients.end(),
                     [&ws](const Client &client) { return client.ws == ws; });

    if (it == clients.end()) {
        throw std::runtime_error("Client not found");
    }

    return it;
}

static auto findClientNotLock(const WebSocket &ws) {
    auto it =
        std::find_if(clients.begin(), clients.end(),
                     [&ws](const Client &client) { return client.ws == ws; });

    if (it == clients.end()) {
        throw std::runtime_error("Client not found");
    }

    return it;
}

static void broadcast(const std::string &message) {
    for (auto &client : clients) {
        std::scoped_lock<std::mutex> lock(client.mtx);
        client.ws.sendFrame(message.c_str(), static_cast<int>(message.size()),
                            WebSocket::FRAME_TEXT);
    }
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

    for (auto &client : clients) {
        std::scoped_lock<std::mutex> lock(client.mtx);
        client.ws.sendFrame(message.c_str(), static_cast<int>(message.size()),
                            WebSocket::FRAME_TEXT);
    }
}

static auto initMessage(const std::string &type) {
    Poco::JSON::Object::Ptr obj = new Poco::JSON::Object;
    obj->set("type", type);
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
    msg->set("opcoes", opcoes);

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

            if (obj->has("type") &&
                obj->getValue<std::string>("type") == "seleciona_op") {
                auto op = obj->getValue<int>("op");
                auto retn = obj->getNullableValue<int>("retn").value(0);

                messages.pop();

                *iOpcaoSelecionada = op;
                std::cout << __func__ << ": " << *iOpcaoSelecionada
                          << std::endl;
                return retn;
            } else {
                messages.pop();
                std::cout << __func__ << ": "
                          << "Invalid message: " << message.message
                          << std::endl;

                std::scoped_lock<std::mutex> lck(clientsMtx);

                auto cli = findClientNotLock(message.ws);
                std::scoped_lock<std::mutex> lck2(cli->mtx);

                auto msgToCli = jsonToStr(msg);
                cli->ws.sendFrame(msgToCli.c_str(),
                                  static_cast<int>(msgToCli.size()),
                                  WebSocket::FRAME_TEXT);
            }
        } catch (const std::exception &e) {
            std::cerr << "Error parsing message: " << e.what() << ": "
                      << message.message << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
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

    try {
        auto obj = Poco::JSON::Parser()
                       .parse(message.message)
                       .extract<Poco::JSON::Object::Ptr>();
        if (obj->has("type")) {
            auto type = obj->getValue<std::string>("type");

            if (type == "transacaoDebito") {
                messages.pop();
                lock.unlock();
                auto valor = obj->getValue<std::string>("valor");
                auto cupom = obj->getValue<std::string>("cupom");

                char buffControle[128]{};

                auto retn = integ.TransacaoCartaoDebito(
                    valor.data(), cupom.data(), buffControle);

                auto msgToCli = initMessage("transacaoDebito");
                msgToCli->set("retn", retn);
                msgToCli->set("numeroControle", std::string(buffControle));

                broadcastJson(msgToCli);
            } else if (type == "confirma") {
                messages.pop();
                lock.unlock();
                auto numeroControle =
                    obj->getValue<std::string>("numeroControle");

                auto retn = integ.ConfirmaCartao(numeroControle.data());

                auto msgToCli = initMessage("confirma");
                msgToCli->set("retn", retn);

                broadcastJson(msgToCli);
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

static void runMessages() {
    while (!stop.load()) {
        processMessages();
    }
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

int main() {
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);

    integ.carregaDll();
    integ.setCallBackDisplayErro(display_error);
    integ.setCallBackDisplayMensagem(display_msg);
    integ.setCallBackDisplayTerminal(display_terminal);
    integ.setCallBackBeep(callback_beep);
    integ.setCallBackSelecionaOpcao(callback_seleciona_op);

    try {
        // Set up HTTP server to handle WebSocket requests
        ServerSocket svs(9000); // Listen on port 9000
        HTTPServer server(new WebSocketRequestHandlerFactory, svs,
                          new HTTPServerParams);

        server.start(); // Start the server
        std::cout << "WebSocket server started on port 9000" << std::endl;

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
