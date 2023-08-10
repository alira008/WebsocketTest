#include "message.hpp"
#include "pch.hpp"
#include "websocket.hpp"

void handleMessage(std::span<const char> message) {
  fmt::print("Received bytes: {}\n", message.size());
  // for (auto c : message) {
  //   fmt::print("{}", c);
  // }
  // fmt::print("\n");
}

int main() {
  websocket_test::MessageHandler message_handler(handleMessage);
  websocket_test::Websocket ws(message_handler, true);
  ws.Connect("stream.data.alpaca.markets", "/v2/iex");
  // ws.ReadMessage();
  nlohmann::json authMessage;
  authMessage["action"] = "auth";
  authMessage["key"] = "";
  authMessage["secret"] = "";

  nlohmann::json subscribeMessage;
  subscribeMessage["action"] = "subscribe";
  // subscribeMessage["trades"] = {"AAPL"};
  subscribeMessage["quotes"] = {"AAL"};
  // subscribeMessage["bars"] = {"*"};

  ws.SendMessage(authMessage.dump());
  ws.SendMessage(subscribeMessage.dump());
  ws.ReadMessages();

  return 0;
}
