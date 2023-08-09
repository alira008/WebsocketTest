#include "pch.hpp"
#include "allocator.hpp"

namespace websocket_test {

namespace net = boost::asio;
namespace beast = boost::beast;
namespace websockets = beast::websocket;
namespace ssl = net::ssl;

class Websocket {
 public:
  explicit Websocket(bool is_secure = false) {
    connection_state_ = ConnectionState::kInitial;
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

  enum ConnectionState {
    kInitial,
    kResolving,
    kConnectingTcp,
    kSslHandshake,
    kUpgradeWs,
    kConnected,
    kError
  };

 private:
  using tcp = net::ip::tcp;
  using stream = websockets::stream<beast::ssl_stream<beast::tcp_stream>>;

  std::unique_ptr<net::io_context> io_context_;
  std::unique_ptr<ssl::context> ssl_context_;
  std::unique_ptr<stream> socket_;
  tcp::resolver::results_type results_;
  ConnectionState connection_state_;
  std::string host_;
  std::string host_end_point_;
  std::string service_;
  std::array<char, 800000> buf_;
  MemoryHandler memory_handler_;

  net::awaitable<void> ConnectToServer();
  net::awaitable<void> WebsocketStateStep();
  net::awaitable<void> Send(std::string_view message);
  net::awaitable<void> ReadSingle();
  net::awaitable<void> Read();
};

}  // namespace websocket_test
