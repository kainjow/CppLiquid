cmake_minimum_required(VERSION 3.1)
project(CppLiquid)

option(CPPLIQUID_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

macro(add_exe STRING_TYPE)
  set(SRCS
    src/liquid/block.cpp
    src/liquid/block.hpp
    src/liquid/blockbody.cpp
    src/liquid/blockbody.hpp
    src/liquid/context.cpp
    src/liquid/context.hpp
    src/liquid/data.cpp
    src/liquid/data.hpp
    src/liquid/drop.cpp
    src/liquid/drop.hpp
    src/liquid/error.hpp
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
    src/liquid/tag.hpp
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
    src/liquid/tags/break.cpp
    src/liquid/tags/break.hpp
    src/liquid/tags/capture.cpp
    src/liquid/tags/capture.hpp
    src/liquid/tags/case.cpp
    src/liquid/tags/case.hpp
    src/liquid/tags/comment.cpp
    src/liquid/tags/comment.hpp
    src/liquid/tags/continue.cpp
    src/liquid/tags/continue.hpp
    src/liquid/tags/cycle.cpp
    src/liquid/tags/cycle.hpp
    src/liquid/tags/decrement.cpp
    src/liquid/tags/decrement.hpp
    src/liquid/tags/for.cpp
    src/liquid/tags/for.hpp
    src/liquid/tags/if.cpp
    src/liquid/tags/if.hpp
    src/liquid/tags/ifchanged.cpp
    src/liquid/tags/ifchanged.hpp
    src/liquid/tags/increment.cpp
    src/liquid/tags/increment.hpp
    src/liquid/string.hpp
    src/liquid/stringref.hpp
    src/liquid/string_std.hpp
    src/liquid/string_std.cpp
    src/liquid/string_qt.hpp
  )
  add_library(${PROJECT_NAME}-${STRING_TYPE} ${SRCS})

  target_compile_definitions(${PROJECT_NAME}-${STRING_TYPE} PRIVATE
    LIQUID_STRING_USE_${STRING_TYPE}
  )

  target_include_directories(${PROJECT_NAME}-${STRING_TYPE} PRIVATE
    src/liquid
    src/liquid/tags
  )

  if(CPPLIQUID_TESTS)
    add_executable(${PROJECT_NAME}-${STRING_TYPE}-Test
      tests/main.cpp
      tests/tests.hpp
      ${SRCS}
    )

    target_compile_definitions(${PROJECT_NAME}-${STRING_TYPE}-Test PRIVATE
      TESTS
      LIQUID_STRING_USE_${STRING_TYPE}
    )

    target_include_directories(${PROJECT_NAME}-${STRING_TYPE}-Test PRIVATE
      src/liquid
      src/liquid/tags
      tests
    )
  endif()

  if(MSVC)
    target_compile_options(${PROJECT_NAME}-${STRING_TYPE} PRIVATE
      /WX
      /W4
      /wd4714 # Ignore "xxx marked as __forceinline not inlined" for Qt's toLower() and toUpper()
      /sdl
    )
  else()
    target_compile_options(${PROJECT_NAME}-${STRING_TYPE} PRIVATE
      -Wall
      -Wextra
      -Werror
    )
  endif()
  
  enable_testing()
  add_test(NAME ${PROJECT_NAME}-${STRING_TYPE} COMMAND ${PROJECT_NAME}-${STRING_TYPE}-Test)
endmacro()

add_exe(STD)

find_package(Qt5 COMPONENTS Core)
if(Qt5_FOUND)
  add_exe(QT)  
  target_link_libraries(${PROJECT_NAME}-QT Qt5::Core)
  target_link_libraries(${PROJECT_NAME}-QT-Test Qt5::Core)
  if(MSVC)
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
		get_target_property(QtCore_location Qt5::Core LOCATION_${CMAKE_BUILD_TYPE})
		set(output_dir ${CMAKE_BUILD_TYPE})
	else()
		get_target_property(QtCore_location Qt5::Core LOCATION_Debug)
		set(output_dir Debug)
	endif()
    file(COPY ${QtCore_location} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${output_dir})
  endif()
endif()

source_group(liquid\\tags REGULAR_EXPRESSION src/liquid/tags/*)
source_group(liquid REGULAR_EXPRESSION src/liquid/*)
