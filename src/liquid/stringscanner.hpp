#ifndef LIQUID_STRINGSCANNER_HPP
#define LIQUID_STRINGSCANNER_HPP

#include "string.hpp"
#include "stringutils.hpp"

namespace Liquid {

    class StringScanner {
    public:
        StringScanner(const StringRef& input, StringRef::size_type pos = 0)
            : input_(input)
            , pos_(pos)
        {
        }
        
        bool skipWhitespace() {
            const auto size = input_.size();
            bool skipped = false;
            while (pos_ < size && isSpace(input_.at(pos_))) {
                ++pos_;
                skipped = true;
            }
            return skipped;
        }
        
        StringRef scanStringLiteral() {
            const auto size = input_.size();
            if (pos_ >= size) {
                return StringRef();
            }
            const auto first = input_.at(pos_);
            if (first != '\"' && first != '\'') {
                return StringRef();
            }
            const StringRef::size_type startPos = pos_ + 1;
            StringRef::size_type pos = startPos;
            StringRef::size_type count = 0;
            for (; pos < size; ++pos) {
                if (input_.at(pos) == first) {
                    const StringRef str = input_.mid(startPos, count);
                    pos_ += count + 2;
                    return str;
                }
                ++count;
            }
            return StringRef();
        }
        
        StringRef scanIdentifier() {
            StringRef::size_type count = 0;
            const auto size = input_.size();
            for (StringRef::size_type i = pos_; i < size; ++i, ++count) {
                const auto ch = input_.at(i);
                if (count == 0) {
                    if (!(isLetter(ch) || ch == '_')) {
                        return StringRef();
                    }
                } else if (!(isLetter(ch) || isDigit(ch) || ch == '_' || ch == '-')) {
                    break;
                }
            }
            const StringRef result = input_.mid(pos_, count);
            pos_ += count;
            return result;
        }
        
        StringRef peekInt() {
            StringRef::size_type count = 0;
            const auto size = input_.size();
            for (StringRef::size_type i = pos_ ; i < size; ++i, ++count) {
                const auto ch = input_.at(i);
                if (count == 0) {
                    if (!(isDigit(ch) || (ch == '-' && i < (size - 1) && isDigit(input_.at(i + 1))))) {
                        return StringRef();
                    }
                } else if (!isDigit(ch)) {
                    break;
                }
            }
            return input_.mid(pos_, count);
        }

        StringRef scanInt() {
            const StringRef intStr = peekInt();
            if (!intStr.isEmpty()) {
                pos_ += intStr.size();
            }
            return intStr;
        }

        StringRef scanFloat() {
            const StringRef intStr = peekInt();
            if (intStr.isEmpty()) {
                return intStr;
            }
            const auto size = input_.size();
            auto count = intStr.size();
            auto pos = pos_ + count;
            if (pos > (size - 1) || input_.at(pos) != '.') {
                return StringRef();
            }
            ++pos;
            ++count;
            for (StringRef::size_type i = pos ; i < size; ++i, ++count) {
                const auto ch = input_.at(i);
                if (!isDigit(ch)) {
                    break;
                }
            }
            const StringRef result = input_.mid(pos_, count);
            pos_ += count;
            return result;
        }
        
        bool scanUpTo(const String& string) {
            const auto index = input_.indexOf(string, pos_);
            if (index == -1) {
                return false;
            }
            pos_ = index;
            return true;
        }
        
        bool scanString(const String& string, StringRef* result = nullptr) {
            const StringRef mid = input_.mid(pos_, string.size());
            if (mid == string) {
                pos_ += string.size();
                if (result) {
                    *result = mid;
                }
                return true;
            }
            return false;
        }
        
        bool eof() const {
            return pos_ >= input_.size();
        }
        
        StringRef getch() {
            const StringRef str = input_.mid(pos_, 1);
            ++pos_;
            return str;
        }
        
        StringRef peekch(StringRef::size_type distance = 0) {
            const auto pos = pos_ + distance;
            if (pos >= input_.size()) {
                return StringRef();
            }
            return input_.mid(pos, 1);
        }
        
        void setPosition(StringRef::size_type pos) {
            pos_ = pos;
        }
        
        void advance(int num) {
            pos_ += num;
        }
        
        StringRef::size_type position() const {
            return pos_;
        }
        
    private:
        const StringRef& input_;
        StringRef::size_type pos_;
        
        template <typename T>
        bool isDigit(const T ch) const {
            return ch >= '0' && ch <= '9';
        };
        
        template <typename T>
        bool isLetter(const T ch) const {
            return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
        };

    };

}

#endif
