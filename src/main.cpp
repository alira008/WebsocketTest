#include "websocket/websocket.hpp"
#include <iostream>
int main() {
  std::cout << "hello world\n";
  websocket::Websocket socket;
  socket.Connect("www.google.com");

  return 0;
}
