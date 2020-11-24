get_filename_component(PROJECT_NAME_STR ${CMAKE_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${PROJECT_NAME_STR})

cmake_minimum_required(VERSION 2.8)
project(${PROJECT_NAME_STR})

#----------------------------------------
# Application
#----------------------------------------
aux_source_directory(. SRC_LIST)

# Headers
file(GLOB HEADERS_LIST "*.h" "*.hpp")
add_executable(${PROJECT_NAME} ${SRC_LIST} ${HEADERS_LIST})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

#----------------------------------------
# Tests
#----------------------------------------
enable_testing()
add_test(tests ${PROJECT_NAME})