#ifndef LIQUID_EXPRESSION_HPP
#define LIQUID_EXPRESSION_HPP

#include <QVariant>
#include <vector>

namespace Liquid {
    
    class Expression {
    public:
        enum class Type {
            Nil,
            String,
            NumberInt,
            NumberFloat,
            BooleanTrue,
            BooleanFalse,
            LookupKey,
            LookupExpression,
        };
        
        Expression(Type type)
            : type_(type)
        {
        }
        
        Expression(const Expression& other)
            : type_(other.type_)
            , var_(other.var_)
        {
        }
        
        Expression& operator=(const Expression& other) {
            if (this != &other) {
                type_ = other.type_;
                var_ = other.var_;
            }
            return *this;
        }
        
        bool operator==(const Expression& other) const {
            return type_ == other.type_ && var_ == other.var_;
        }
        
        Expression()
            : type_(Type::Nil)
        {
        }
        
        Expression(const QString& string)
            : type_(Type::String)
            , var_(string)
        {
        }
        
        Expression(const char *string) : Expression(QString(string)) {}

        Expression(int value)
            : type_(Type::NumberInt)
            , var_(value)
        {
        }

        Expression(double value)
            : type_(Type::NumberFloat)
            , var_(value)
        {
        }

        Expression(bool value)
            : type_(value ? Type::BooleanTrue : Type::BooleanFalse)
        {
        }

        Type type() const {
            return type_;
        }
        
        bool isString() const {
            return type_ == Type::String;
        }

        bool isNumber() const {
            return type_ == Type::NumberInt || type_ == Type::NumberFloat;
        }
        
        bool isInt() const {
            return type_ == Type::NumberInt;
        }
        
        bool isFloat() const {
            return type_ == Type::NumberFloat;
        }

        bool isBoolean() const {
            return type_ == Type::BooleanTrue || type_ == Type::BooleanFalse;
        }
        
        bool isNil() const {
            return type_ == Type::Nil;
        }
        
        bool isLookupKey() const {
            return type_ == Type::LookupKey;
        }
        
        QString toString() const {
            return var_.toString();
        }
        
        bool toBool() const {
            return type_ == Type::BooleanTrue;
        }
        
        int toInt() const {
            return var_.toInt();
        }
        
        double toFloat() const {
            return var_.toDouble();
        }
        
        void setLookupKey(const QString& key) {
            var_ = key;
        }
        
        QString lookupKey() const {
            return var_.toString();
        }
        
        static Expression parse(const QStringRef& input);
        
    private:
        Type type_;
        QVariant var_;
        std::vector<Expression> lookups_;
    };

}

#endif
