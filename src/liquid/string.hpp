#ifndef LIQUID_STRING_HPP
#define LIQUID_STRING_HPP

#include <unordered_map>

#if defined(LIQUID_STRING_USE_QT)
#include "string_qt.hpp"
#elif defined(LIQUID_STRING_USE_STD)
#include "string_std.hpp"
#else
#error "LIQUID_STRING_USE_STD or LIQUID_STRING_USE_QT must be defined!"
#endif
#include "stringref.hpp"

#endif
