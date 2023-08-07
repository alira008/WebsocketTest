# Name:   nlohmann_json Link:   https://github.com/nlohmann/json Purpose:    For
# encoding json requests
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(json)
