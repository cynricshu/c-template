cmake_minimum_required(VERSION 3.13)
project(c++template)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(src)

include_directories(/usr/local/include)
include_directories(../../baidu/base/baidu-rpc/src)
include_directories(../../baidu/third-party/json-cpp/include)
include_directories(../../baidu/base/common)
include_directories(../../baidu/bce-audio/kaldi/src)

aux_source_directory(src/common SRC_COMMON)
aux_source_directory(src/web SRC_WEB)

add_library(c++template
        ${SRC_COMMON}
        ${SRC_WEB}
        )

add_executable(cartesian_product
        src/demo/cartesian_product.cpp)
add_executable(time
        src/demo/time.cpp)
add_executable(rvo
        src/demo/rvo.cpp )
target_compile_options(rvo PUBLIC -fno-elide-constructors)