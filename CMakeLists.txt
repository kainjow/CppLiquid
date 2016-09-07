cmake_minimum_required(VERSION 3.1)
project(Jeqyll)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core)

add_executable(${PROJECT_NAME}
  tests/main.cpp

  src/liquid/block.cpp
  src/liquid/block.hpp
  src/liquid/blockbody.cpp
  src/liquid/blockbody.hpp
  src/liquid/context.cpp
  src/liquid/context.hpp
  src/liquid/data.cpp
  src/liquid/data.hpp
  src/liquid/expression.cpp
  src/liquid/expression.hpp
  src/liquid/filter.hpp
  src/liquid/lexer.cpp
  src/liquid/lexer.hpp
  src/liquid/node.cpp
  src/liquid/node.hpp
  src/liquid/parser.cpp
  src/liquid/parser.hpp
  src/liquid/standardfilters.cpp
  src/liquid/standardfilters.hpp
  src/liquid/stringscanner.cpp
  src/liquid/stringscanner.hpp
  src/liquid/stringutils.cpp
  src/liquid/stringutils.hpp
  src/liquid/template.cpp
  src/liquid/template.hpp
  src/liquid/token.cpp
  src/liquid/token.hpp
  src/liquid/tokenizer.cpp
  src/liquid/tokenizer.hpp
  src/liquid/variable.cpp
  src/liquid/variable.hpp
  src/liquid/tags/assign.cpp
  src/liquid/tags/assign.hpp
  src/liquid/tags/capture.cpp
  src/liquid/tags/capture.hpp
  src/liquid/tags/comment.cpp
  src/liquid/tags/comment.hpp
  src/liquid/tags/decrement.cpp
  src/liquid/tags/decrement.hpp
  src/liquid/tags/increment.cpp
  src/liquid/tags/increment.hpp
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  TESTS
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
  src/liquid
  src/liquid/tags
  tests
)

if(MSVC)
  get_target_property(QtCore_location Qt5::Core LOCATION_Debug)
  file(COPY ${QtCore_location} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
endif()

source_group(liquid\\tags REGULAR_EXPRESSION src/liquid/tags/*)
source_group(liquid REGULAR_EXPRESSION src/liquid/*)

target_link_libraries(${PROJECT_NAME} Qt5::Core)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE
	/WX
	/W3
  )
else()
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
  )
endif()
