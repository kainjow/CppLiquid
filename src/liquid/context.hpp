#ifndef LIQUID_CONTEXT_HPP
#define LIQUID_CONTEXT_HPP

#include <QString>
#include <QHash>
#include <QList>

namespace Liquid {

    class Context {
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
        };
        
        using Hash = QHash<QString, Context>;
        using Array = QList<Context>;
        
        Context()
            : type_(Type::Nil)
        {
        }
        
        Context(Type type)
            : type_(type)
        {
        }
        
        Context(const Context& ctx)
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
            }
        }
        
        Context& operator=(const Context& ctx) {
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
                }
            }
            return *this;
        }
        
        bool operator==(const Context& other) const {
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
            }
        }
        
        explicit Context(const Hash& hash)
            : type_(Type::Hash)
            , hash_(hash)
        {
        }

        explicit Context(const Array& array)
            : type_(Type::Array)
            , array_(array)
        {
        }

        Context(const QString& string)
            : type_(Type::String)
            , string_(string)
        {
        }
        
        Context(const char *string) : Context(QString(string)) {}

        explicit Context(int value)
            : type_(Type::NumberInt)
        {
            number_.i = value;
        }

        explicit Context(double value)
            : type_(Type::NumberFloat)
        {
            number_.f = value;
        }

        explicit Context(bool value)
            : type_(value ? Type::BooleanTrue : Type::BooleanFalse)
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

        bool isBoolean() const {
            return type_ == Type::BooleanTrue || type_ == Type::BooleanFalse;
        }
        
        bool isNil() const {
            return type_ == Type::Nil;
        }
        
        const QString toString() const {
            switch (type_) {
                case Type::BooleanTrue:
                    return "true";
                case Type::BooleanFalse:
                    return "false";
                case Type::NumberInt:
                    return QString::number(number_.i);
                case Type::NumberFloat:
                    return QString::number(number_.f);
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
        
        void push_back(const Context& ctx) {
            if (isArray()) {
                array_.push_back(ctx);
            }
        }
        
        int size() const {
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
        
        const Context& at(int index) const {
            return array_.at(index);
        }

    private:
        Type type_;
        Hash hash_;
        Array array_;
        QString string_;
        union {
            int i;
            double f;
        } number_;
    };

}

#endif
