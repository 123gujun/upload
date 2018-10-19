cmake_minimum_required(VERSION 3.12)
project(upload)

set(CMAKE_CXX_STANDARD 14)

include_directories("/Users/gujun/Downloads/jsoncpp-src-0.5.0/include")

link_directories("/Users/gujun/Downloads/jsoncpp-src-0.5.0/buildscons/linux-gcc-4.2.1/src/lib_json")
link_libraries("/usr/lib/libcurl.4.dylib" "libjson_linux-gcc-4.2.1_libmt.dylib")


add_executable(upload main.cpp upload.cpp upload.h)