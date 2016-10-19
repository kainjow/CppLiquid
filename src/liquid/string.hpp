#ifndef LIQUID_STRING_HPP
#define LIQUID_STRING_HPP

#include <unordered_map>

#if defined(LIQUID_STRING_USE_QT)
#include "string_qt.hpp"
#elif defined(LIQUID_STRING_USE_STD)
#include "string_std.hpp"
#else
#error "A LIQUID_STRING_USE_XXX value must be defined!"
#endif

namespace Liquid {

    class StringRef {
    public:
        
        using value_type = String::value_type;
        using size_type = String::size_type;
        
        StringRef()
            : s_(nullptr)
            , pos_(0)
            , len_(0)
        {
        }
        
        StringRef(const String* str)
            : s_(str)
            , pos_(0)
            , len_(str->size())
        {
        }

        size_type size() const {
            return len_;
        }
        
        bool isEmpty() const {
            return len_ == 0;
        }
        
        value_type at(size_type pos) const {
            return s_->at(pos_ + pos);
        }
        
        StringRef mid(size_type /*pos*/, size_type /*num*/ = -1) const {
            return {};
        }
        
        StringRef left(size_type /*num*/) const {
            return {};
        }
        
        size_type indexOf(const String& /*str*/, size_type from = String::npos) const {
            (void)from;
            return -1;
        }
        
        bool endsWith(const String&/* str*/) const {
            return false;
        }
        
        String toString() const {
            return {};
        }
        
        StringRef& operator=(const StringRef& other) {
            if (&other != this) {
                s_ = other.s_;
                pos_ = other.pos_;
                len_ = other.len_;
            }
            return *this;
        }
        
        bool operator==(const StringRef& /*other*/) const {
            return false;
        }
        
        bool operator==(const String& /*other*/) const {
            return false;
        }
        
        bool operator!=(const String& /*other*/) const {
            return false;
        }
        
        int toInt() const {
            return 0;
        }
        
        double toDouble() const {
            return 0;
        }
        
        StringRef trimmed() const {
            return {};
        }

    private:
        const String *s_;
        size_type pos_;
        size_type len_;
    };

    String intToString(int value);

}

#endif
