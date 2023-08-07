# Name:   boost Link:   https://github.com/boostorg/boost Purpose:    Contains
# common useful libraries Comments:   Make sure this is installed in your
# environment Installation:   Can be downloaded from website/github Libraries
# that will be used: Beast/Asio: For setting up websocket client
find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost library not found!!")
endif()
