#include "pch.hpp"
// #include "websocket/websocket.hpp"
#include "websocket.hpp"
int main() {
  websocket_test::Websocket ws;
  ws.Connect("stream.data.sandbox.alpaca.markets", "/v2/iex");
  // // socket.Send("hello");
  // socket.ReadMessage();

  return 0;
}
