#include "message.hpp"
#include "pch.hpp"
#include "websocket.hpp"

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

  boost::asio::io_context io_context;
  auto ssl_context =
      boost::asio::ssl::context(boost::asio::ssl::context::tlsv12_client);

  boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
  signals.async_wait([&](auto, auto) { io_context.stop(); });

  new_websocket_test::Websocket ws(io_context, ssl_context);
  boost::asio::co_spawn(io_context, ws.StartWebsocket(api_key, secret),
                        [](std::exception_ptr e) {
                          if (e) {
                            try {
                              std::rethrow_exception(e);
                            } catch (std::exception& e) {
                              fmt::print(stderr, "Error: {}\n", e.what());
                            }
                          }
                        });

  // Run the I/O service.
  io_context.run();

  fmt::print("Program stopped\n");

  return EXIT_SUCCESS;
}
