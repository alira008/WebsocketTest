#include <iostream>

#include "websocket/websocket.hpp"
int main() {
  std::cout << "hello world\n";
  websocket::Websocket socket;
  socket.Connect("paper-api.alpaca.markets");

  return 0;
}
