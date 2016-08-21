#ifndef LIQUID_CONTEXT_HPP
#define LIQUID_CONTEXT_HPP

#include <QDebug>
#include <QHash>
#include <QList>
#include <QString>
#include "expression.hpp"

namespace Liquid {

    class Context;
    
    const extern Context kNilContext;

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
        
        Context(std::nullptr_t nil)
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
        
        Context(const Hash& hash)
            : type_(Type::Hash)
            , hash_(hash)
        {
        }

        Context(const Array& array)
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

        Context(int value)
            : type_(Type::NumberInt)
        {
            number_.i = value;
        }

        Context(double value)
            : type_(Type::NumberFloat)
        {
            number_.f = value;
        }

        Context(bool value)
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
            if (index >= array_.size()) {
                return kNilContext;
            }
            return array_.at(index);
        }
        
        void insert(const QString& key, const Context& value) {
            hash_.insert(key, value);
        }
        
        const Context& operator[](const QString& key) const {
            const Hash::const_iterator it = hash_.find(key);
            if (it == hash_.end()) {
                return kNilContext;
            }
            return it.value();
        }
        
        const Context& evaluate(const Expression& expression) const {
            return Context::evaluate(expression, *this);
        }
        
        static const Context& evaluate(const Expression& expression, const Context& rootCtx) {
            if (expression.isLookupKey()) {
                if (rootCtx.isHash()) {
                    const Context& result = rootCtx[expression.key()];
                    if (!result.isNil()) {
                        return result;
                    }
                }
            } else if (expression.isLookup() || expression.isLookupBracketKey()) {
                const Context* currentCtx = &rootCtx;
                for (const auto& lookup : expression.lookups()) {
                    if (lookup.isLookupBracketKey()) {
                        const Context& bracketResult = Context::evaluate(lookup, rootCtx);
                        if (bracketResult.isString() && currentCtx->isHash()) {
                            const Context& result = (*currentCtx)[bracketResult.toString()];
                            if (result.isNil()) {
                                return result;
                            }
                            currentCtx = &result;
                        } else if (bracketResult.isNumberInt() && currentCtx->isArray()) {
                            const Context& result = currentCtx->at(bracketResult.toInt());
                            if (result.isNil()) {
                                return result;
                            }
                            currentCtx = &result;
                        } else {
                            return kNilContext;
                        }
                    } else {
                        const Context& result = currentCtx->evaluate(lookup);
                        if (result.isNil()) {
                            return result;
                        }
                        currentCtx = &result;
                    }
                }
                return *currentCtx;
            } else if (expression.isString()) {
                return rootCtx.addTemporary(Context(expression.toString()));
            } else if (expression.isInt()) {
                return rootCtx.addTemporary(Context(expression.toInt()));
            } else {
                throw QString("Can't evaluate expression %1").arg(expression.typeString()).toStdString();
            }
            return kNilContext;
        }
        
        const Context& addTemporary(const Context& tmp) const {
            temporaries_.push_back(tmp);
            return temporaries_.back();
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
        mutable std::vector<Context> temporaries_;
    };

}

#endif
