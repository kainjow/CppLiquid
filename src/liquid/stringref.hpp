#ifndef LIQUID_STRINGREF_HPP
#define LIQUID_STRINGREF_HPP

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
        
        StringRef(const String* str, size_type position, size_type length)
            : s_(str)
            , pos_(position)
            , len_(length)
        {
        }

        size_type size() const {
            return len_;
        }
        
        bool isNull() const {
            return s_ == nullptr;
        }
        
        bool isEmpty() const {
            return len_ == 0;
        }
        
        value_type at(size_type pos) const {
            return s_->at(pos_ + pos);
        }
        
        StringRef mid(size_type pos, size_type num = -1) const {
            const auto sz = size();
            if (pos > sz) {
                return {};
            }
            const size_type len = num == static_cast<size_type>(-1) || sz < num ? sz - pos : num;
            return StringRef(s_, pos_ + pos, len);
        }
        
        StringRef left(size_type num) const {
            if (num >= size()) {
                return StringRef(s_, pos_, len_);
            }
            return StringRef(s_, pos_, num);
        }
        
        size_type indexOf(const String& str, size_type from = 0) const {
            const auto sz = size() - str.size();
            for (size_type i = from; i < sz; ++i) {
                if (mid(i, str.size()) == str) {
                    return i;
                }
            }
            return -1;
        }
        
        String toString() const {
            if (isNull()) {
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
        
        bool operator==(const StringRef& other) const {
            const auto sz = size();
            if (sz != other.size()) {
                return false;
            }
            for (size_type i = 0; i < sz; ++i) {
                if (other.at(i) != at(i)) {
                    return false;
                }
            }
            return true;
        }
        
        bool operator==(const String& other) const {
            return operator==(&other);
        }
        
        bool operator!=(const String& other) const {
            return !operator==(other);
        }
        
        int toInt() const {
            return std::stoi(toString().toStdString());
        }
        
        double toDouble() const {
            return std::stod(toString().toStdString());
        }
        
    private:
        const String *s_;
        size_type pos_;
        size_type len_;
    };

}

#endif
