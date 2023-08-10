#include "websocket.hpp"

#include "nlohmann/json.hpp"

namespace websocket_test {

void Websocket::SendMessage(std::string_view message) {
  net::co_spawn(*io_context_, Send(message), [](std::exception_ptr e) {
    if (e) {
      try {
        std::rethrow_exception(e);
      } catch (std::exception& e) {
        fmt::print(stderr, "Error: {}\n", e.what());
      }
    }
  });
  io_context_->run();
  io_context_->reset();
}

net::awaitable<void> Websocket::Send(std::string_view message) {
  fmt::print("Sending message: {}\n", message);

  size_t bytes =
      co_await socket_->async_write(net::buffer(message), net::use_awaitable);

  fmt::print("Sent: {} bytes\n", bytes);
}

void Websocket::ReadMessage() {
  net::co_spawn(*io_context_, ReadSingle(), [](std::exception_ptr e) {
    if (e) {
      try {
        std::rethrow_exception(e);
      } catch (std::exception& e) {
        fmt::print(stderr, "Error: {}\n", e.what());
      }
    }
  });
  io_context_->run();
  io_context_->reset();
}

void Websocket::ReadMessages() {
  net::co_spawn(*io_context_, Read(), [](std::exception_ptr e) {
    if (e) {
      try {
        std::rethrow_exception(e);
      } catch (std::exception& e) {
        fmt::print(stderr, "Error: {}\n", e.what());
      }
    }
  });
  io_context_->run();
  io_context_->reset();
}

net::awaitable<void> Websocket::ReadSingle() {
  size_t total_bytes = 0;
  do {
    size_t bytes = co_await socket_->async_read_some(
        net::buffer(buf_), net::bind_allocator(Allocator<char>(memory_handler_),
                                               net::use_awaitable));
    total_bytes += bytes;
  } while (!socket_->is_message_done());

  message_handler_.OnMessage({buf_.data(), total_bytes});
  buf_.fill(0);
}

net::awaitable<void> Websocket::Read() {
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> prev;
    using std::chrono::microseconds;
  while (socket_->is_open()) {
    start = std::chrono::high_resolution_clock::now();
    co_await ReadSingle();
    auto duration = duration_cast<microseconds>(start - prev);
    fmt::print("Timer {}\n", duration);
    prev = start;
  }
}

void Websocket::Connect(std::string_view host, std::string_view end_point) {
  fmt::print("Connecting to host: {}{}\n", host, end_point);
  host_ = host;
  host_end_point_ = end_point;

  io_context_ = std::make_unique<net::io_context>();
  net::co_spawn(*io_context_, ConnectToServer(), [](std::exception_ptr e) {
    if (e) {
      try {
        std::rethrow_exception(e);
      } catch (std::exception& e) {
        fmt::print(stderr, "Error: {}\n", e.what());
      }
    }
  });
  io_context_->run();
  io_context_->reset();
}

net::awaitable<void> Websocket::ConnectToServer() {
  ssl_context_ = std::make_unique<ssl::context>(ssl::context::tlsv12_client);
  auto resolver = net::use_awaitable.as_default_on(
      tcp::resolver(co_await net::this_coro::executor));
  results_ =
      co_await resolver.async_resolve(host_, service_, net::use_awaitable);
  socket_ = std::make_unique<stream>(co_await net::this_coro::executor,
                                     *ssl_context_);

  co_await beast::get_lowest_layer(*socket_).async_connect(results_,
                                                           net::use_awaitable);
  fmt::print("Established tcp connection.\n");

  // Set SNI Hostname
  SSL_set_tlsext_host_name(socket_->next_layer().native_handle(),
                           host_.c_str());

  co_await socket_->next_layer().async_handshake(ssl::stream_base::client,
                                                 net::use_awaitable);
  fmt::print("Successfully performed ssl handshake\n");

  co_await socket_->async_handshake(host_, host_end_point_, net::use_awaitable);
  fmt::print("Connected to server\n");
}

}  // namespace websocket_test
