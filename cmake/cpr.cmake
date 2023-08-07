# Name:   curl for people Link:   https://github.com/libcpr/cpr Purpose:   For
# making simple http requests
find_package(cpr REQUIRED)
if(NOT cpr_FOUND)
  message(FATAL_ERROR "lib cpr library not found!!")
endif()
