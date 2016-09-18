#ifndef LIQUID_DATA_HPP
#define LIQUID_DATA_HPP

#include <QDebug>
#include <memory>
#include <vector>
#include "stringutils.hpp"
#include "qstringhash.hpp"
#include "drop.hpp"

namespace Liquid {

    class Data;
    
    const extern Data kNilData;
    const extern Data kTrueData;
    const extern Data kFalseData;

    class Data {
    public:
        enum class Type {
            Hash,
            Array,
            String,
            NumberInt,
            NumberFloat,
            BooleanTrue,
            BooleanFalse,
            Nil,
            Drop,
        };
        
        using Hash = std::unordered_map<QString, Data, QStringHash>;
        using Array = std::vector<Data>;
        
        Data()
            : type_(Type::Nil)
        {
        }
        
        Data(std::nullptr_t)
            : type_(Type::Nil)
        {
        }
        
        Data(Type type)
            : type_(type)
        {
        }
        
        Data(const Data& ctx)
            : type_(ctx.type_)
        {
            switch (type_) {
                case Type::Hash:
                    hash_ = ctx.hash_;
                    break;
                case Type::Array:
                    array_ = ctx.array_;
                    break;
                case Type::String:
                    string_ = ctx.string_;
                    break;
                case Type::NumberInt:
                    number_.i = ctx.number_.i;
                    break;
                case Type::NumberFloat:
                    number_.f = ctx.number_.f;
                    break;
                case Type::BooleanTrue:
                case Type::BooleanFalse:
                case Type::Nil:
                    break;
                case Type::Drop:
                    drop_ = ctx.drop_;
                    break;
            }
        }
        
        Data& operator=(const Data& ctx) {
            if (this != &ctx) {
                type_ = ctx.type_;
                switch (type_) {
                    case Type::Hash:
                        hash_ = ctx.hash_;
                        break;
                    case Type::Array:
                        array_ = ctx.array_;
                        break;
                    case Type::String:
                        string_ = ctx.string_;
                        break;
                    case Type::NumberInt:
                        number_.i = ctx.number_.i;
                        break;
                    case Type::NumberFloat:
                        number_.f = ctx.number_.f;
                        break;
                    case Type::BooleanTrue:
                    case Type::BooleanFalse:
                    case Type::Nil:
                        break;
                    case Type::Drop:
                        drop_ = ctx.drop_;
                }
            }
            return *this;
        }
        
        bool operator==(const Data& other) const {
            if (type_ != other.type_) {
                return false;
            }
            switch (type_) {
                case Type::Hash:
                    return hash_ == other.hash_;
                case Type::Array:
                    return array_ == other.array_;
                case Type::String:
                    return string_ == other.string_;
                case Type::NumberInt:
                    return number_.i == other.number_.i;
                case Type::NumberFloat:
                    return number_.f == other.number_.f;
                case Type::BooleanTrue:
                case Type::BooleanFalse:
                case Type::Nil:
                    return true;
                case Type::Drop:
                    return drop_ == other.drop_;
                default:
                    return false;
            }
        }
        
        bool operator!=(const Data& other) {
            return !(*this == other);
        }
        
        Data(const Hash& hash)
            : type_(Type::Hash)
            , hash_(hash)
        {
        }

        Data(const Array& array)
            : type_(Type::Array)
            , array_(array)
        {
        }

        Data(const QString& string)
            : type_(Type::String)
            , string_(string)
        {
        }
        
        Data(const char *string) : Data(QString(string)) {}

        Data(int value)
            : type_(Type::NumberInt)
        {
            number_.i = value;
        }

        Data(double value)
            : type_(Type::NumberFloat)
        {
            number_.f = value;
        }

        Data(bool value)
            : type_(value ? Type::BooleanTrue : Type::BooleanFalse)
        {
        }
        
        Data(const std::shared_ptr<Drop>& drop)
            : type_(Type::Drop)
            , drop_(drop)
        {
        }

        Type type() const {
            return type_;
        }
        
        bool isHash() const {
            return type_ == Type::Hash;
        }
        
        bool isArray() const {
            return type_ == Type::Array;
        }

        bool isString() const {
            return type_ == Type::String;
        }

        bool isNumber() const {
            return type_ == Type::NumberInt || type_ == Type::NumberFloat;
        }

        bool isNumberInt() const {
            return type_ == Type::NumberInt;
        }

        bool isNumberFloat() const {
            return type_ == Type::NumberFloat;
        }

        bool isBoolean() const {
            return type_ == Type::BooleanTrue || type_ == Type::BooleanFalse;
        }
        
        bool isNil() const {
            return type_ == Type::Nil;
        }
        
        bool isDrop() const {
            return type_ == Type::Drop;
        }
        
        QString toString() const {
            switch (type_) {
                case Type::BooleanTrue:
                    return "true";
                case Type::BooleanFalse:
                    return "false";
                case Type::NumberInt:
                    return QString::number(number_.i);
                case Type::NumberFloat:
                    return doubleToString(number_.f);
                default:
                    return string_;
            }
        }
        
        bool toBool() const {
            return type_ == Type::BooleanTrue ? true : false;
        }

        int toInt() const {
            switch (type_) {
                case Type::NumberInt:
                    return number_.i;
                case Type::NumberFloat:
                    return number_.f;
                default:
                    return 0;
            }
        }

        double toFloat() const {
            switch (type_) {
                case Type::NumberInt:
                    return number_.i;
                case Type::NumberFloat:
                    return number_.f;
                default:
                    return 0;
            }
        }
        
        bool isTruthy() const {
            switch (type_) {
                case Type::Nil:
                case Type::BooleanFalse:
                    return false;
                default:
                    return true;
            }
        }
        
        void push_back(const Data& obj) {
            if (!isArray()) {
                throw std::string("push_back() requires an array");
            }
            if (isArray()) {
                array_.push_back(obj);
            }
        }
        
        void pop_back() {
            if (!isArray()) {
                throw std::string("pop_back() requires an array");
            }
            if (array_.empty()) {
                throw std::string("pop_back() cannot be used on an empty array");
            }
            array_.pop_back();
        }
        
        size_t size() const {
            switch (type_) {
                case Type::Hash:
                    return hash_.size();
                case Type::Array:
                    return array_.size();
                case Type::String:
                    return string_.size();
                default:
                    return 0;
            }
        }
        
        const Data& at(size_t index) const {
            if (!isArray()) {
                throw std::string("at() requires an array");
            }
            if (index >= array_.size()) {
                return kNilData;
            }
            return array_.at(index);
        }
        
        const Array& array() const {
            if (!isArray()) {
                throw std::string("array() requires an array");
            }
            return array_;
        }
        
        void insert(const QString& key, const Data& value) {
            if (!isHash()) {
                throw std::string("insert() requires a hash");
            }
            hash_[key] = value;
        }
        
        const Data& operator[](const QString& key) const {
            if (isHash()) {
                const auto it = hash_.find(key);
                if (it == hash_.end()) {
                    return kNilData;
                }
                return it->second;
            } else if (isDrop()) {
                return (*drop_)[key];
            } else {
                throw std::string("[] requires a hash or drop");
            }
        }
        
        bool containsKey(const QString& key) const {
            if (!isHash()) {
                throw std::string("containsKey requires a hash");
            }
            return hash_.find(key) != hash_.end();
        }
        
        std::shared_ptr<Drop> drop() const {
            if (!isDrop()) {
                throw std::string("drop() requires a drop");
            }
            return drop_;
        }
        
    private:
        Type type_;
        Hash hash_;
        Array array_;
        QString string_;
        std::shared_ptr<Drop> drop_;
        union {
            int i;
            double f;
        } number_;
    };

}

#endif
