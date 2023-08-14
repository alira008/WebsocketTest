// Internal libraries
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

//  External libraries
#include <cpr/cpr.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
// #include <readerwritercircularbuffer.h>
// #include "simdjson.h"

#define BOOST_ASIO_HAS_CO_AWAIT
#include "boost/asio.hpp"
#include "boost/asio/awaitable.hpp"
#include "boost/asio/co_spawn.hpp"
#include "boost/asio/detached.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/asio/use_awaitable.hpp"
#include "boost/beast.hpp"
#include "boost/beast/ssl.hpp"
#include "nlohmann/json.hpp"
