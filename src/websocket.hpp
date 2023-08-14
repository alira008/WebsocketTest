#pragma once

#include "allocator.hpp"
#include "message.hpp"
#include "pch.hpp"

namespace websocket_test {

namespace net = boost::asio;
namespace beast = boost::beast;
namespace websockets = beast::websocket;
namespace ssl = net::ssl;

class Websocket {
 public:
  explicit Websocket(MessageHandler message_handler, bool is_secure = false)
      : message_handler_(message_handler) {
    service_ = is_secure ? "https" : "http";
  }

  // Attempt to connect to socket
  void Connect(std::string_view host, std::string_view end_point);

  // Send Message to socket
  void SendMessage(std::string_view message);

  // Read Message from socket
  void ReadMessage();

  // Read Messages from socket
  void ReadMessages();

 private:
  using tcp = net::ip::tcp;
  using stream = websockets::stream<beast::ssl_stream<beast::tcp_stream>>;

  std::unique_ptr<net::io_context> io_context_;
  std::unique_ptr<ssl::context> ssl_context_;
  std::unique_ptr<stream> socket_;
  tcp::resolver::results_type results_;
  std::string host_;
  std::string host_end_point_;
  std::string service_;
  std::array<char, 800000> buf_;
  MemoryHandler memory_handler_;
  MessageHandler message_handler_;

  net::awaitable<void> ConnectToServer();
  net::awaitable<void> WebsocketStateStep();
  net::awaitable<void> Send(std::string_view message);
  net::awaitable<void> ReadSingle();
  net::awaitable<void> Read();
};

struct ManualTimer {
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> stop;
  ManualTimer() { start = std::chrono::high_resolution_clock::now(); }
  void Stop() { stop = std::chrono::high_resolution_clock::now(); }
  void Print(const char* msg = "") {
    using std::chrono::microseconds;
    auto duration = duration_cast<microseconds>(stop - start);
    fmt::print("{}{}\n", msg, duration);
  }
};

}  // namespace websocket_test
