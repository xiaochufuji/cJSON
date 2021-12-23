CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

project(cjson VERSION 1.0)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(main
                ${PROJECT_SOURCE_DIR}/src/cjson.c
                ${PROJECT_SOURCE_DIR}/src/test.c
)