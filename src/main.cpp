#include "message.hpp"
#include "pch.hpp"
#include "websocket.hpp"

void handleMessage(std::span<const char> message) {
  fmt::print("Received bytes: {}\n", message.size());
}

std::map<std::string, std::string> load_env(const std::string& filename) {
  std::map<std::string, std::string> env_vars;
  std::ifstream file(filename);
  std::string line;

  while (std::getline(file, line)) {
    std::istringstream line_stream(line);
    std::string key, value;

    if (std::getline(line_stream, key, '=') &&
        std::getline(line_stream, value)) {
      env_vars[key] = value;
    }
  }

  return env_vars;
}

int main() {
  auto env_vars = load_env(".env");
  std::string api_key = env_vars["API_KEY"];
  std::string secret = env_vars["SECRET"];

  websocket_test::MessageHandler message_handler(handleMessage);
  websocket_test::Websocket ws(message_handler, true);
  ws.Connect("stream.data.alpaca.markets", "/v2/iex");
  // ws.ReadMessage();
  nlohmann::json authMessage;
  authMessage["action"] = "auth";
  authMessage["key"] = api_key;
  authMessage["secret"] = secret;

  nlohmann::json subscribeMessage;
  subscribeMessage["action"] = "subscribe";
  subscribeMessage["quotes"] = {"AAL"};

  ws.SendMessage(authMessage.dump());
  ws.SendMessage(subscribeMessage.dump());
  ws.ReadMessages();

  return 0;
}
