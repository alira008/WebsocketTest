#include "websocket.hpp"
#include <iostream>

namespace websocket {

int Websocket::Connect(std::string_view host) {
  std::cout << "Connecting to host: " << host << "\n";

  io_context_ = std::make_unique<net::io_context>();
  ioc_strand_ = std::make_unique<net::io_context::strand>(*io_context_);
  resolver_ = std::make_unique<tcp::resolver>(net::make_strand(*io_context_));

  resolver_->async_resolve(
      host, "https", beast::bind_front_handler(&Websocket::OnResolve, this));

  io_context_->run();
  return 1;
}

int Websocket::Send(std::string_view message) {
  std::cout << "Sending message: " << message << "\n";

  return 1;
}

void Websocket::OnResolve(beast::error_code ec,
                          tcp::resolver::results_type results) {
  if (ec) {
    std::cout << "Could not resolve host\n";
    return;
  }

  // Initiate ssl
  ssl_context_ = std::make_unique<ssl::context>(ssl::context::tlsv12_client);

  // Initiate socket
  socket_ =
      std::make_unique<stream>(net::make_strand(*io_context_), *ssl_context_);

  // Set timeout for the next operation
  beast::get_lowest_layer(*socket_).expires_after(std::chrono::seconds(30));
  beast::get_lowest_layer(*socket_).async_connect(
      results, beast::bind_front_handler(&Websocket::OnTcpConnect, this));
}

void Websocket::OnTcpConnect(beast::error_code ec,
                             tcp::resolver::endpoint_type end_point) {
  boost::ignore_unused(end_point);

  if (ec) {
    std::cout << "Error on connect: " << ec.message() << "\n";
    return;
  }
  std::cout << "Successfully made a connection\n";

  //  Perform the ssl handshake
  socket_->next_layer().async_handshake(
      ssl::stream_base::client,
      net::bind_executor(*ioc_strand_, beast::bind_front_handler(
                                           &Websocket::OnSslHandshake, this)));
}

void Websocket::OnSslHandshake(beast::error_code ec) {
  if (ec) {
    std::cout << "Error on connect: " << ec.message() << "\n";
    return;
  }

  std::cout << "Successfuly made ssl handshake";
}

} // namespace websocket
