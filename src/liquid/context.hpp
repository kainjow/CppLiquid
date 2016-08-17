#ifndef LIQUID_CONTEXT_HPP
#define LIQUID_CONTEXT_HPP

#include <QString>
#include <QHash>
#include <QList>

namespace {
    const QString kBooleanTrueString = "true";
    const QString kBooleanFalseString = "false";
}

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

        explicit Context(const QString& string)
            : type_(Type::String)
            , string_(string)
        {
        }
        
        explicit Context(const char *string) : Context(QString(string)) {}

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
        
        const QString& toString() const {
            switch (type_) {
                case Type::BooleanTrue:
                    return kBooleanTrueString;
                case Type::BooleanFalse:
                    return kBooleanFalseString;
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

    private:
        const Type type_;
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
