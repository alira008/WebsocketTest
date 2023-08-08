#pragma WEBSOCKET_HPP

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <memory>
#include <string>
#include <string_view>

namespace websocket {

namespace net = boost::asio;
namespace beast = boost::beast;
namespace websockets = beast::websocket;
namespace ssl = net::ssl;

class Websocket {
 public:
  Websocket() { buffer_.reserve(1024); }

  // Attempt to connect to socket
  int Connect(std::string_view host);

  // Send Message to api
  int Send(std::string_view message);

 private:
  using tcp = net::ip::tcp;
  using stream = websockets::stream<beast::ssl_stream<beast::tcp_stream>>;

  std::unique_ptr<net::io_context> io_context_;
  std::unique_ptr<tcp::resolver> resolver_;
  std::unique_ptr<ssl::context> ssl_context_;
  std::unique_ptr<stream> socket_;
  std::unique_ptr<net::io_context::strand> ioc_strand_;
  beast::flat_buffer buffer_;
  std::string host_;

  void OnResolve(beast::error_code ec, tcp::resolver::results_type results);
  void OnTcpConnect(beast::error_code, tcp::resolver::endpoint_type);
  void OnSslHandshake(beast::error_code);
  void OnHandshake(beast::error_code);
};
}  // namespace websocket
