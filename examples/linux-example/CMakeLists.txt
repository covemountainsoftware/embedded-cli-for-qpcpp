set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Werror)
include_directories(${CMS_CHAR_DEVICE_INCLUDE})

find_package(Threads REQUIRED)
add_executable(linux-example main.cpp)
target_link_libraries(linux-example PRIVATE qpcpp Threads::Threads cms-embedded-cli-service)