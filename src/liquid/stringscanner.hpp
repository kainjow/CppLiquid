#ifndef LIQUID_STRINGSCANNER_HPP
#define LIQUID_STRINGSCANNER_HPP

#include <QDebug>
#include <QString>

namespace Liquid {

    class StringScanner {
    public:
        StringScanner(const QStringRef& input)
            : input_(input)
            , pos_(0)
        {
        }
        
        void skipWhitespace() {
            const int size = input_.size();
            while (pos_ < size && input_.at(pos_).isSpace()) {
                ++pos_;
            }
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
            const auto isLetter = [](const ushort ch) {
                return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
            };
            for (int i = pos_; i < size; ++i, ++count) {
                const ushort ch = input_.at(i).unicode();
                if (count == 0) {
                    if (!(isLetter(ch) || ch == '_')) {
                        return QStringRef();
                    }
                } else if (!(isLetter(ch) || isDigit(ch) || ch == '_' || ch == '-')) {
                    break;
                }
            }
            const QStringRef result = input_.mid(pos_, count);
            pos_ += count;
            return result;
        }
        
        QStringRef peekInt() {
            int count = 0;
            const int size = input_.size();
            for (int i = pos_ ; i < size; ++i, ++count) {
                const ushort ch = input_.at(i).unicode();
                if (count == 0) {
                    if (!(isDigit(ch) || (ch == '-' && i < (size - 1) && isDigit(input_.at(i + 1).unicode())))) {
                        return QStringRef();
                    }
                } else if (!isDigit(ch)) {
                    break;
                }
            }
            return input_.mid(pos_, count);
        }

        QStringRef scanInt() {
            const QStringRef intStr = peekInt();
            if (!intStr.isNull()) {
                pos_ += intStr.size();
            }
            return intStr;
        }

        QStringRef scanFloat() {
            const QStringRef intStr = peekInt();
            if (intStr.isNull()) {
                return intStr;
            }
            const int size = input_.size();
            int count = intStr.size();
            int pos = pos_ + count;
            if (pos > (size - 1) || input_.at(pos).unicode() != '.') {
                return QStringRef();
            }
            ++pos;
            ++count;
            for (int i = pos ; i < size; ++i, ++count) {
                const ushort ch = input_.at(i).unicode();
                if (!isDigit(ch)) {
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
        
        bool isDigit(const ushort ch) const {
            return ch >= '0' && ch <= '9';
        };
    };

}

#endif
