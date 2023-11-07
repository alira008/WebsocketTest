#include "pch.hpp"

namespace new_websocket_test {

class Websocket {
 public:
  Websocket(boost::asio::io_context& io_context,
            boost::asio::ssl::context& ssl_context)
      : io_context_(io_context), ssl_context_(ssl_context) {}

  boost::asio::awaitable<void> Connect(std::string_view host,
                                       std::string_view endpoint, bool secure);
  boost::asio::awaitable<void> Send(std::string_view message);
  boost::asio::awaitable<void> ReadMessage();
  boost::asio::awaitable<void> ReadMessages();
  boost::asio::awaitable<void> StartWebsocket(std::string_view api_key,
                                              std::string_view secret) {
    co_await Connect("stream.data.alpaca.markets", "/v2/iex", true);

    nlohmann::json authMessage;
    authMessage["action"] = "auth";
    authMessage["key"] = api_key;
    authMessage["secret"] = secret;

    nlohmann::json subscribeMessage;
    subscribeMessage["action"] = "subscribe";
    subscribeMessage["quotes"] = {"AAL", "AMZN"};

    co_await Send(authMessage.dump());
    co_await Send(subscribeMessage.dump());
    co_await ReadMessages();
  }

 private:
  boost::asio::io_context& io_context_;
  boost::asio::ssl::context& ssl_context_;
  std::unique_ptr<boost::beast::websocket::stream<
      boost::beast::ssl_stream<boost::beast::tcp_stream>>>
      ws_;
  std::array<char, 800000> buf_;
};

}  // namespace new_websocket_test
