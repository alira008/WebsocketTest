# Name:   MoodyCamel::ConcurrentQueue Link:
# https://github.com/cameron314/concurrentqueue Purpose:    High performance
# multi producer multi consumer queue for websocket client
FetchContent_Declare(
  concurrentqueue
  GIT_REPOSITORY https://github.com/cameron314/concurrentqueue
  GIT_TAG v1.0.3
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(concurrentqueue)
