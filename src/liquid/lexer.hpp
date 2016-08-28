#ifndef LIQUID_LEXER_HPP
#define LIQUID_LEXER_HPP

#include <QDebug>
#include <QString>
#include <QRegularExpression>
#include <vector>

namespace Liquid {

    class Token {
    public:
        enum class Type {
            Invalid,
            
            Pipe,
            Dot,
            Colon,
            Comma,
            OpenSquare,
            CloseSquare,
            OpenRound,
            CloseRound,
            Question,
            Dash,
            Comparison,
            String,
            NumberInt,
            NumberFloat,
            Id,
            Dotdot,
            EndOfString,
        };
        
        static QString typeToString(Type type) {
            switch (type) {
                case Type::Invalid: return "Invalid";
                case Type::Pipe: return "Pipe";
                case Type::Dot: return "Dot";
                case Type::Colon: return "Colon";
                case Type::Comma: return "Comma";
                case Type::OpenSquare: return "OpenSquare";
                case Type::CloseSquare: return "CloseSquare";
                case Type::OpenRound: return "OpenRound";
                case Type::CloseRound: return "CloseRound";
                case Type::Question: return "Question";
                case Type::Dash: return "Dash";
                case Type::Comparison: return "Comparison";
                case Type::String: return "String";
                case Type::NumberInt: return "NumberInt";
                case Type::NumberFloat: return "NumberFloat";
                case Type::Id: return "Identifier";
                case Type::Dotdot: return "DotDot";
                case Type::EndOfString: return "EndOfString";
            }
        }
        
        Token()
            : type_(Type::Invalid)
        {
        }
        
        Token(Type type, const QStringRef& value)
            : type_(type)
            , value_(value)
        {
        }
        
        Type type() const {
            return type_;
        }
        
        const QStringRef& value() const {
            return value_;
        }
        
        bool isValid() const {
            return type_ != Type::Invalid;
        }
        
    private:
        Type type_;
        QStringRef value_;
    };
    
    const extern Token kTokenInvalid;
    
    class StringScanner {
    public:
        StringScanner(const QStringRef& input)
            : input_(input)
            , pos_(0)
        {
        }
        
        void skip(const QRegularExpression& regex) {
            (void)scan(regex);
        }
        
        void skipWhitespace() {
            const int size = input_.size();
            while (pos_ < size && input_.at(pos_).isSpace()) {
                ++pos_;
            }
        }
        
        QStringRef scan(const QRegularExpression& regex) {
            if (!regex.isValid()) {
                return QStringRef();
            }
            const QRegularExpressionMatch match = regex.match(input_, pos_, QRegularExpression::NormalMatch, QRegularExpression::AnchoredMatchOption);
            if (!match.isValid()) {
                return QStringRef();
            }
            // Don't use capturedRef() as that's relative to QRegEx's copy of input :(
            const int capturedLen = match.capturedLength();
            if (capturedLen == 0) {
                return QStringRef();
            }
            const QStringRef captured = input_.mid(pos_, capturedLen);
            pos_ += capturedLen;
            return captured;
        }
        
        QStringRef scanStringLiteral() {
            const int size = input_.size();
            if (pos_ >= size) {
                return QStringRef();
            }
            const QChar first = input_.at(pos_);
            if (first != '\"' && first != '\'') {
                return QStringRef();
            }
            const int startPos = pos_ + 1;
            int pos = startPos;
            int count = 0;
            for (; pos < size; ++pos) {
                if (input_.at(pos) == first) {
                    const QStringRef str = input_.mid(startPos, count);
                    pos_ += count + 2;
                    return str;
                }
                ++count;
            }
            return QStringRef();
        }
        
        QStringRef scanIdentifier() {
            int count = 0;
            const int size = input_.size();
            for (int i = pos_; i < size; ++i, ++count) {
                const ushort ch = input_.at(i).unicode();
                if (count == 0) {
                    if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')) {
                        return QStringRef();
                    }
                } else if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '-')) {
                    break;
                }
            }
            const QStringRef result = input_.mid(pos_, count);
            pos_ += count;
            return result;
        }
        
        bool eof() const {
            return pos_ >= input_.size();
        }
        
        QStringRef getch() {
            const QStringRef str = input_.mid(pos_, 1);
            ++pos_;
            return str;
        }
        
        QStringRef peekch(int distance = 0) {
            const int pos = pos_ + distance;
            if (pos >= input_.size()) {
                return QStringRef();
            }
            return input_.mid(pos, 1);
        }
        
        void setPosition(int pos) {
            pos_ = pos;
        }
        
        int position() const {
            return pos_;
        }
        
    private:
        const QStringRef& input_;
        int pos_;
    };

    class Lexer {
    public:
        static std::vector<Token> tokenize(const QStringRef& input);
    };

}

#endif
