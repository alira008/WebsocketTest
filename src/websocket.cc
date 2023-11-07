#include "websocket.hpp"

namespace new_websocket_test {

constexpr auto use_nothrow_awaitable =
    boost::asio::as_tuple(boost::asio::use_awaitable);

boost::asio::awaitable<void> Websocket::Connect(std::string_view host,
                                                std::string_view endpoint,
                                                bool secure) {
  auto resolver = boost::asio::ip::tcp::resolver(io_context_);
  ws_ = std::make_unique<boost::beast::websocket::stream<
      boost::beast::ssl_stream<boost::beast::tcp_stream>>>(io_context_,
                                                           ssl_context_);

  auto [e1, results] = co_await resolver.async_resolve(
      host, secure ? "https" : "http", use_nothrow_awaitable);

  // check if we have correctly resolved host
  if (e1) {
    fmt::print(stderr, "Error resolving host: {}\n", e1.message());
    co_return;
  }

  // make a connection with the ip address we get from resolving the host
  auto [e2, socket] =
      co_await boost::beast::get_lowest_layer(*ws_).async_connect(
          results, use_nothrow_awaitable);

  // check if we made a successful connection
  if (e2) {
    fmt::print(stderr, "Error connecting to host: {}\n", e2.message());
    co_return;
  }

  // Set SNI Hostname
  SSL_set_tlsext_host_name(ws_->next_layer().native_handle(), host.data());

  // perform ssl handshake
  auto [e3] = co_await ws_->next_layer().async_handshake(
      boost::asio::ssl::stream_base::client, use_nothrow_awaitable);

  // perform the websocket handshake
  auto [e4] =
      co_await ws_->async_handshake(host, endpoint, use_nothrow_awaitable);

  // check if we made a successful handshake
  if (e4) {
    fmt::print(stderr, "Error performing handshake: {}\n", e3.message());
    co_return;
  }

  fmt::print("Successfully connected to host: {}\n", host);
}

boost::asio::awaitable<void> Websocket::Send(std::string_view message) {
  auto [e1, bytes_sent] = co_await ws_->async_write(
      boost::asio::buffer(message), use_nothrow_awaitable);

  if (e1) {
    fmt::print(stderr, "Error sending message: {}\n", e1.message());
    co_return;
  }

  fmt::print("Successfully sent message: {}\n", message);
}

boost::asio::awaitable<void> Websocket::ReadMessage() {
  size_t total_bytes = 0;
  do {
    auto [e1, bytes] = co_await ws_->async_read_some(boost::asio::buffer(buf_),
                                                     use_nothrow_awaitable);
    total_bytes += bytes;
  } while (!ws_->is_message_done());

  fmt::print("Successfully read message: {}\n", buf_.data());
  fmt::print("Received bytes: {}\n", total_bytes);
  buf_.fill(0);
}

boost::asio::awaitable<void> Websocket::ReadMessages() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> prev;
  using std::chrono::microseconds;
  while (ws_->is_open()) {
    start = std::chrono::high_resolution_clock::now();
    co_await ReadMessage();
    auto duration = duration_cast<microseconds>(start - prev);
    fmt::print("Timer {}\n", duration);
    prev = start;
  }
}

}  // namespace new_websocket_test
