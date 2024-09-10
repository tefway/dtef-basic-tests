#include "ClasseIntegracao.h"
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerRequestImpl.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/WebSocket.h>
#include <Poco/ThreadPool.h>
#include <atomic>
#include <csignal>
#include <deque>
#include <iostream>
#include <mutex>
#include <queue>
#include <shared_mutex>
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

static void broadcast(const std::string &message) {
    for (auto &client : clients) {
        std::scoped_lock<std::mutex> lock(client.mtx);
        client.ws.sendFrame(message.c_str(), static_cast<int>(message.size()),
                            WebSocket::FRAME_TEXT);
    }
}

static void *CALLING_COV display_error(char *msg) {
    std::cout << __func__ << ": " << msg << std::endl;
    return msg;
}

static void *CALLING_COV display_msg(char *msg) {
    std::cout << __func__ << ": " << msg << std::endl;
    return msg;
}

static void *CALLING_COV display_terminal(char *msg) {
    std::cout << __func__ << ": " << msg << std::endl;
    return msg;
}

static void *CALLING_COV callback_beep() {
    std::cout << __func__ << std::endl;
    return nullptr;
}

static void runMessages() {
    while (!stop.load()) {
        std::unique_lock<std::shared_mutex> lock(messagesMtx);
        if (!messages.empty()) {
            auto message = messages.front();
            lock.unlock();

            try {
                broadcast(message.message);
            } catch (const std::exception &e) {
                std::cerr << "Error broadcasting message: " << e.what()
                          << std::endl;
            }

            lock.lock();
            messages.pop();
        }

        Thread::sleep(100);
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

            while (!stop.load() && ws.available()) {
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

    std::thread messagesThread(runMessages);

    try {
        // Set up HTTP server to handle WebSocket requests
        ServerSocket svs(9000); // Listen on port 9000
        HTTPServer server(new WebSocketRequestHandlerFactory, svs,
                          new HTTPServerParams);

        server.start(); // Start the server
        std::cout << "WebSocket server started on port 9000" << std::endl;

        // Keep the server running
        while (!stop.load()) {
            Thread::sleep(1000);
        }
    } catch (Poco::Exception &exc) {
        std::cerr << "Server error: " << exc.displayText() << std::endl;
    }

    if (messagesThread.joinable()) {
        messagesThread.join();
    }

    return 0;
}
