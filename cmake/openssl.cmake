# Name:   openssl Link:   https://github.com/openssl/openssl Purpose:    For ssl
# and tls in http and websocket requests Comments:   Make sure this is installed
# in your environment Installation:   Can be downloaded from their
# website/github, brew, pacman
if(APPLE)
  set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()

set(OPENSSL_USE_STATIC_LIBS ON)
find_package(OpenSSL REQUIRED COMPONENTS Crypto SSL)
if(NOT OPENSSL_FOUND)
  message(FATAL_ERROR "OpenSSL library not found!!")
endif()
