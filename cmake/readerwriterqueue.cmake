# Name:   ReaderWriterQueue Link:
# https://github.com/cameron314/readerwriterqueue Purpose:    High performance
# single producer single consumer queue for websocket client
FetchContent_Declare(
  readerwriterqueue
  GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue
  GIT_TAG master
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(readerwriterqueue)
