# Name:   simdjson Link:   https://github.com/simdjson/simdjson Purpose:    High
# performance json reader
FetchContent_Declare(
  simdjson
  GIT_REPOSITORY https://github.com/simdjson/simdjson.git
  GIT_TAG v3.0.1
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(simdjson)
