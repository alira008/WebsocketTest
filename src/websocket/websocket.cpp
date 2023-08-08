#include "websocket.hpp"

namespace websocket {

void Websocket::Connect(std::string_view host, std::string_view end_point) {
  fmt::print("Connecting to host: {}{}\n", host, end_point);
  host_ = host;
  end_point_ = end_point;

  io_context_ = std::make_unique<net::io_context>();
  ioc_strand_ = std::make_unique<net::io_context::strand>(*io_context_);
  resolver_ = std::make_unique<tcp::resolver>(net::make_strand(*io_context_));

  resolver_->async_resolve(
      host_, "https", beast::bind_front_handler(&Websocket::OnResolve, this));

  io_context_->run();
}

void Websocket::Send(std::string_view message) {
  fmt::print("Sending message: {}\n", message);

  socket_->async_write(
      net::buffer(message),
      net::bind_executor(*ioc_strand_, [](beast::error_code ec,
                                          std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec) {
          fmt::print(stderr, "Error on send: {}\n", ec.message());
          return;
        }

        fmt::print("Sent: {} bytes\n", bytes_transferred);
      }));

  io_context_->run();
  io_context_->reset();
}

void Websocket::ReadMessage() {
  buffer_.consume(buffer_.size());
  // auto resolver = net::use_awaitable.as_default_on(
  //     tcp::resolver(co_await net::this_coro::executor));
  // auto ws = net::use_awaitable.as_default_on(
  //     websockets::stream<beast::tcp_stream>(co_await net::this_coro::executor));
  // co_await socket_->async_read(buffer_);
  // co_await ws.async_read_some(
  //     buffer_, 1024, net::use_awaitable);

  // socket_->async_read(
  //     buffer_,
  //       beast::bind_front_handler(&Websocket::OnReadSome, this));

  io_context_->run();
  io_context_->reset();
}

void Websocket::OnReadSome(const beast::error_code& ec,
                           std::size_t bytes_written) {
  if (ec) {
    fmt::print(stderr, "Could not read bytes.");
  }
  fmt::print("Bytes written: {}\n", bytes_written);
}

void Websocket::OnResolve(beast::error_code ec,
                          tcp::resolver::results_type results) {
  if (ec) {
    fmt::print(stderr, "Could not resolve host\n");
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
    fmt::print(stderr, "Error trying to connect: {}\n", ec.message());
    return;
  }

  // Set SNI Hostname
  if (!SSL_set_tlsext_host_name(socket_->next_layer().native_handle(),
                                host_.c_str())) {
    ec = beast::error_code(static_cast<int>(::ERR_get_error()),
                           net::error::get_ssl_category());
    return;
  }

  // Set port in host
  host_ += ':' + std::to_string(end_point.port());

  //  Perform the ssl handshake
  socket_->next_layer().async_handshake(
      ssl::stream_base::client,
      net::bind_executor(*ioc_strand_, beast::bind_front_handler(
                                           &Websocket::OnSslHandshake, this)));
}

void Websocket::OnSslHandshake(beast::error_code ec) {
  if (ec) {
    fmt::print(stderr, "Error making ssl handshake: {}\n", ec.message());
    return;
  }

  socket_->async_handshake(
      host_, end_point_,
      net::bind_executor(*ioc_strand_, beast::bind_front_handler(
                                           &Websocket::OnHandshake, this)));
}

void Websocket::OnHandshake(beast::error_code ec) {
  if (ec) {
    fmt::print(stderr, "Error making handshake: {}\n", ec.message());
  }

  fmt::print("Connected to websocket\n");
}

}  // namespace websocket
