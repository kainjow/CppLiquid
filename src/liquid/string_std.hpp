#ifndef LIQUID_STRING_STD_HPP
#define LIQUID_STRING_STD_HPP

#ifndef LIQUID_STRING_USE_STD
#error "LIQUID_STRING_USE_STD must be defined to use this file!"
#endif

#include <string>
#include <unordered_map>
#include <vector>

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
            return {};
        }
        
        String toUpper() const {
            return {};
        }
        
        String mid(size_type /*pos*/, size_type num = -1) const {
            (void)num;
            return {};
        }
        
        String left(size_type /*num*/) const {
            return {};
        }
        
        String trimmed() const {
            return {};
        }
        
        String replace(const String& /*before*/, const String& /*after*/) const {
            return {};
        }
        
        String replace(size_type /*pos*/, const String& /*before*/, const String& /*after*/) const {
            return {};
        }
        
        String toHtmlEscaped() const {
            return {};
        }
        
        size_type indexOf(value_type /*ch*/, size_type from = 0) const {
            (void)from;
            return -1;
        }
        
        size_type indexOf(const String& /*str*/, size_type from = 0) const {
            (void)from;
            return -1;
        }
        
        std::vector<String> split(const String& delimiter) const {
            std::vector<String> result;
            if (delimiter.isEmpty()) {
                result.push_back(*this);
                return result;
            }
            std::string::const_iterator substart = s_.begin();
            for (;;) {
                const auto subend = search(substart, s_.end(), delimiter.s_.begin(), delimiter.s_.end());
                result.push_back(base(substart, subend));
                if (subend == s_.end()) {
                    break;
                }
                substart = subend + delimiter.size();
            }
            return result;
        }
        
        bool compare(const String& other, bool caseSensitive) const {
            if (caseSensitive) {
                return false;
            }
            return s_ == other.s_;
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

};

#endif
