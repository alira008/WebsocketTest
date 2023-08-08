#include "websocket.hpp"

#include "fmt/color.h"

namespace websocket_test {

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
}

net::awaitable<void> Websocket::ConnectToServer() {
  while (connection_state_ != ConnectionState::kConnected &&
         connection_state_ != ConnectionState::kError) {
    co_await WebsocketStateStep();
  }
}

net::awaitable<void> Websocket::WebsocketStateStep() {
  switch (connection_state_) {
    case ConnectionState::kInitial:
      ssl_context_ =
          std::make_unique<ssl::context>(ssl::context::tlsv12_client);

      // Next State
      connection_state_ = ConnectionState::kResolving;
      break;
    case ConnectionState::kResolving:
      auto resolver = net::use_awaitable.as_default_on(
          tcp::resolver(co_await net::this_coro::executor));
      results_ =
          co_await resolver.async_resolve(host_, service_, net::use_awaitable);

      // Next State
      connection_state_ = ConnectionState::kConnectingTcp;
      break;
    case ConnectionState::kConnectingTcp:
      socket_ = std::make_unique<stream>(co_await net::this_coro::executor,
                                         *ssl_context_);
      auto end_point = co_await beast::get_lowest_layer(*socket_).async_connect(
          results_, net::use_awaitable);

      fmt::print("Endpoint: {}\n", end_point.address().to_string());

      // Set SNI Hostname
      SSL_set_tlsext_host_name(socket_->next_layer().native_handle(),
                               host_.c_str());

      // Set port in host
      host_ += ':' + std::to_string(end_point.port());

      // Next State
      connection_state_ = ConnectionState::kSslHandshake;
      break;
    case ConnectionState::kSslHandshake:
      co_await socket_->next_layer().async_handshake(ssl::stream_base::client,
                                                     net::use_awaitable);
      fmt::print("Successfully performed ssl handshake\n");

      // Next State
      connection_state_ = ConnectionState::kUpgradeWs;
      break;
    case ConnectionState::kUpgradeWs:
      co_await socket_->async_handshake(host_, host_end_point_,
                                        net::use_awaitable);

      // Next State
      connection_state_ = ConnectionState::kConnected;
      break;
    case ConnectionState::kConnected:
      fmt::print("Connected to server\n");

      break;
    case ConnectionState::kError:
      break;
  }
}

}  // namespace websocket_test
