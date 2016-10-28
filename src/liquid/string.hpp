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
        
        StringRef(const String* str, int position, int length)
            : s_(str)
            , pos_(position)
            , len_(length)
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
        
        StringRef mid(size_type pos, size_type num = -1) const {
            if (pos > size()) {
                return {};
            }
            const size_type len = num == static_cast<size_type>(-1) || size() < num ? size() - pos : num;
            return StringRef(s_, pos_ + pos, len);
        }
        
        StringRef left(size_type num) const {
            if (num >= size()) {
                return StringRef(s_, pos_, len_);
            }
            return StringRef(s_, pos_, num);
        }
        
        size_type indexOf(const String& /*str*/, size_type from = String::npos) const {
            (void)from;
            return -1;
        }
        
        String toString() const {
            if (!s_) {
                return String();
            }
            return s_->mid(pos_, len_);
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
            return std::stoi(toString().toStdString());
        }
        
        double toDouble() const {
            return std::stod(toString().toStdString());
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
