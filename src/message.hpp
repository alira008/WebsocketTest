#pragma once

#include "pch.hpp"

namespace websocket_test {
struct MessageHandler {
  explicit MessageHandler(std::function<void(std::span<const char>)> on_message)
      : OnMessage(on_message) {}

  std::function<void(std::span<const char>)> OnMessage;
};
}  // namespace websocket_test
