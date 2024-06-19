set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Werror)
include_directories(../../services/embeddedCliService/include)
include_directories(../../drivers/characterDevice/include)

find_package(Threads REQUIRED)
add_executable(linux-example main.cpp ../../services/embeddedCliService/src/embedded_cli_impl.c ../../services/embeddedCliService/src/embeddedCliService.cpp)
target_link_libraries(linux-example PRIVATE  qpcpp Threads::Threads)