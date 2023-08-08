// Internal libraries
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

//  External libraries
#include <cpr/cpr.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <readerwritercircularbuffer.h>
#include <simdjson.h>

#include <boost/asio/strand.hpp>
// #include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <nlohmann/json.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
