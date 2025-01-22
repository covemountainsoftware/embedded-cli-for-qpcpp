add_subdirectory(test)

project(CmsEmbeddedCliService CXX C)

add_library(cms-embedded-cli-service OBJECT
        src/embeddedCliService.cpp
        src/embedded_cli_impl.c
)

target_compile_options(cms-embedded-cli-service PRIVATE -Wall -Wextra -Werror)

target_link_libraries(cms-embedded-cli-service PUBLIC qpcpp)

target_include_directories(cms-embedded-cli-service PRIVATE
        ${CMS_CHAR_DEVICE_INCLUDE}
)

target_include_directories(cms-embedded-cli-service PUBLIC
        include/
)
