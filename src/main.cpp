#include "pch.hpp"
#include "websocket.hpp"

int main() {
  fmt::print("hello world\n");
  websocket_test::Websocket ws(true);
  ws.Connect("stream.data.sandbox.alpaca.markets", "/v2/iex");
  // ws.ReadMessage();
  ws.SendMessage(
      "{\"action\":\"auth\",\"key\":\"\","
      "\"secret\":\"\"}");
  // ws.ReadMessages();
  ws.SendMessage(
      "{\"action\":\"subscribe\",\"trades\":[\"AAPL\"],\"quotes\":[\"AMD\","
      "\"CLDR\"],\"bars\":[\"*\"]}");
  ws.ReadMessages();

  return 0;
}
