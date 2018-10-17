cmake_minimum_required(VERSION 3.12)
project(upload)

set(CMAKE_CXX_STANDARD 14)

#link_directories("/usr/lib/")
link_libraries("/usr/lib/libcurl.4.dylib")


add_executable(upload main.cpp upload.cpp upload.h)