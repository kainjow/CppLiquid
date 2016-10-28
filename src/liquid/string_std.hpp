#ifndef LIQUID_STRING_STD_HPP
#define LIQUID_STRING_STD_HPP

#ifndef LIQUID_STRING_USE_STD
#error "LIQUID_STRING_USE_STD must be defined to use this file!"
#endif

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace Liquid {
    
    class StringRef;
    
    class String {
    private:
        using base = std::string;
        
    public:
        using value_type = base::value_type;
        using size_type = base::size_type;
        
        static const size_type npos = -1;
        
        String() {}
        String(const value_type* ptr) : s_(ptr) {}
        String(const base& str) : s_(str) {}
        String(value_type ch) { s_.append(ch, 1); }
        
        size_type size() const {
            return s_.size();
        }
        
        bool isEmpty() const {
            return s_.empty();
        }
        
        value_type at(size_type pos) const {
            return s_.at(pos);
        }
        
        String& operator=(const String& other) {
            if (&other != this) {
                s_ = other.s_;
            }
            return *this;
        }
        
        bool operator==(const String& other) const {
            return s_ == other.s_;
        }
        
        bool operator!=(const String& other) const {
            return s_ != other.s_;
        }
        
        String operator+(const String& other) const {
            return String(s_ + other.s_);
        }
        
        String& operator+=(const String& other) {
            s_ += other.s_;
            return *this;
        }
        
        String& operator+=(value_type ch) {
            s_ += ch;
            return *this;
        }
        
        const base& raw() const {
            return s_;
        }
        
        String arg(const String& /*arg*/) const {
            return {};
        }

        String arg(const int /*arg*/) const {
            return {};
        }
        
        std::string toStdString() const {
            return s_;
        }
        
        String toLower() const {
            String s;
            const auto sz = size();
            const auto diff = 'a' - 'A';
            for (size_type i = 0; i < sz; ++i) {
                const auto ch = at(i);
                if (ch >= 'A' && ch <= 'Z') {
                    s += (ch + diff);
                } else {
                    s += ch;
                }
            }
            return s;
        }
        
        String toUpper() const {
            String s;
            const auto sz = size();
            const auto diff = 'a' - 'A';
            for (size_type i = 0; i < sz; ++i) {
                const auto ch = at(i);
                if (ch >= 'a' && ch <= 'z') {
                    s += (ch - diff);
                } else {
                    s += ch;
                }
            }
            return s;
        }
        
        String mid(size_type pos, size_type num = -1) const {
            if (pos > size()) {
                return {};
            }
            const size_type len = num == static_cast<size_type>(-1) || size() < num ? size() - pos : num;
            return String(s_.substr(pos, len));
        }
        
        String left(size_type num) const {
            return s_.substr(0, num);
        }
        
        String replace(const String& /*before*/, const String& /*after*/) const {
            return {};
        }
        
        String replace(size_type /*pos*/, const String& /*before*/, const String& /*after*/) const {
            return {};
        }
        
        size_type indexOf(value_type ch, size_type from = 0) const {
            return s_.find(ch, from);
        }
        
        size_type indexOf(const String& str, size_type from = 0) const {
            return s_.find(str.s_, from);
        }
        
        std::vector<String> split(const String& delimiter) const {
            std::vector<String> result;
            if (delimiter.isEmpty()) {
                result.push_back(*this);
                return result;
            }
            auto substart = s_.cbegin();
            for (;;) {
                const auto subend = search(substart, s_.cend(), delimiter.s_.cbegin(), delimiter.s_.cend());
                result.push_back(base(substart, subend));
                if (subend == s_.cend()) {
                    break;
                }
                substart = subend + delimiter.size();
            }
            return result;
        }
        
        int compare(const String& other, bool caseSensitive) const {
            if (!caseSensitive) {
                return -1;
            }
            return s_.compare(other.s_);
        }
        
        StringRef midRef(size_type pos, size_type num = -1) const;

    private:
        base s_;
    };

    struct StringHash {
        std::size_t operator()(const String& k) const {
            return std::hash<std::string>{}(k.raw());
        }
    };
    
    template <typename T>
    using StringKeyUnorderedMap = std::unordered_map<String, T, StringHash>;

    // This is mostly for Catch so it can show Strings natively
    inline std::ostream& operator << (std::ostream& os, const String& value) {
        os << value.toStdString();
        return os;
    }
};

#endif
