#ifndef LIQUID_STRING_QT_HPP
#define LIQUID_STRING_QT_HPP

#ifndef LIQUID_STRING_USE_QT
#error "LIQUID_STRING_USE_QT must be defined to use this file!"
#endif

#include <QString>
#include <QStringList>
#include <QHashFunctions>
#include <unordered_map>
#include <vector>

namespace Liquid {

    class StringRef;
    
    class String {
    private:
        using base = QString;
    
    public:
        using value_type = ushort; // QChar's native type
        using size_type = base::size_type;
        
        static const size_type npos = -1;
        
        String() {}
        String(const char* ptr) : s_(QString::fromUtf8(ptr)) {}
        String(const std::string& str) : String(str.c_str()) {}
        String(const base& str) : s_(str) {}
        String(value_type ch) : s_(1, ch) {}
        
        size_type size() const {
            return s_.size();
        }
        
        bool isEmpty() const {
            return s_.isEmpty();
        }
        
        value_type at(size_type pos) const {
            return s_.at(pos).unicode();
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
        
        String arg(const String& arg) const {
            for (int i = 0; i <= 99; ++i) {
                const auto marker = String("%" + std::to_string(i));
                const auto idx = indexOf(marker);
                if (idx != npos) {
                    return replace(marker, arg);
                }
            }
            return *this;
        }
        
        String arg(const int arg) const {
            return std::to_string(arg);
        }
        
        std::string toStdString() const {
            const auto bytes = s_.toUtf8();
            return std::string{bytes.constData()};
        }
        
        String toLower() const {
            return s_.toLower();
        }
        
        String toUpper() const {
            return s_.toUpper();
        }
        
        String mid(size_type pos, size_type num = -1) const {
            return s_.mid(pos, num);
        }
        
        String left(size_type num) const {
            return mid(0, num);
        }
        
        String replace(const String& before, const String& after) const {
            QString s = s_;
            return s.replace(before.s_, after.s_);
        }
        
        String replace(size_type pos, size_type len, const String& after) const {
            QString s = s_;
            return s.replace(pos, len, after.s_);
        }
        
        size_type indexOf(value_type ch, size_type from = 0) const {
            return s_.indexOf(ch, from);
        }
        
        size_type indexOf(const String& str, size_type from = 0) const {
            return s_.indexOf(str.s_, from);
        }
        
        std::vector<String> split(const String& delimiter) const {
            std::vector<String> result;
            for (const auto& item : s_.split(delimiter.s_)) {
                result.push_back(item);
            }
            return result;
        }
        
        int compare(const String& other, bool caseSensitive) const {
            return s_.compare(other.s_, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
        }
        
        StringRef midRef(size_type pos, size_type num = -1) const;
        
    private:
        base s_;
    };
    
    struct QStringHash {
        std::size_t operator()(const String& k) const {
            return qHash(k.raw());
        }
    };
    
    template <typename T>
    using StringKeyUnorderedMap = std::unordered_map<String, T, QStringHash>;
    
    // This is mostly for Catch so it can show Strings natively
    inline std::ostream& operator << (std::ostream& os, const String& value) {
        os << value.toStdString();
        return os;
    }
};

#endif
