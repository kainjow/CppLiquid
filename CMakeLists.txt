cmake_minimum_required(VERSION 3.1)
project(Jeqyll)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)

add_executable(${PROJECT_NAME}
  src/main.cpp

  src/liquid/component.cpp
  src/liquid/component.hpp
  src/liquid/data.cpp
  src/liquid/data.hpp
  src/liquid/expression.cpp
  src/liquid/expression.hpp
  src/liquid/lexer.cpp
  src/liquid/lexer.hpp
  src/liquid/parser.cpp
  src/liquid/parser.hpp
  src/liquid/stringutils.cpp
  src/liquid/stringutils.hpp
  src/liquid/template.cpp
  src/liquid/template.hpp
  src/liquid/variable.cpp
  src/liquid/variable.hpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  TESTS
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  src/liquid
  tests
)

source_group(liquid REGULAR_EXPRESSION src/liquid/*)

target_link_libraries(${PROJECT_NAME} Qt5::Core)

if (NOT MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Werror
  )
endif()

