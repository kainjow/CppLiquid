cmake_minimum_required(VERSION 3.1)
project(Jeqyll)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)

add_executable(${PROJECT_NAME}
  src/main.cpp

  src/template.cpp
  src/template.hpp
  src/tokenizer.cpp
  src/tokenizer.hpp

  src/tests/tokenizer_unit_test.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  src/tests
)

target_link_libraries(${PROJECT_NAME} Qt5::Core)
