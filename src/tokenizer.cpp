#include "tokenizer.hpp"
#include <QRegularExpression>
#include <QDebug>

Liquid::Tokenizer::Tokenizer(const QString& source)
    : source_(source)
    , line_number_(1)
    , tokens_(tokenize())
{
}

bool Liquid::Tokenizer::shift(QString& token)
{
    if (tokens_.isEmpty()) {
        return false;
    }
    token = tokens_.first();
    tokens_.removeFirst();
    line_number_ += token.count('\n');
    return true;
}

int Liquid::Tokenizer::line_number()
{
    return line_number_;
}

QStringList Liquid::Tokenizer::tokenize()
{
    return split(source_, QRegularExpression("({%.*?%}|{{.*?}})"));
}

QStringList Liquid::Tokenizer::split(const QString& source, const QRegularExpression& regex)
{
    Q_ASSERT(regex.isValid());
    QStringList tokens;
    int pos = 0;
    for (;;) {
        QRegularExpressionMatch match;
        const int find_pos = source.indexOf(regex, pos, &match);
        if (find_pos == -1) {
            break;
        }
        const int diff = find_pos - pos;
        if (diff > 0) {
            tokens << source.mid(pos, diff);
        }
        const QString captured = match.captured();
        tokens << captured;
        pos = find_pos + captured.size();
    }
    if (pos != source.size()) {
        tokens << source.mid(pos);
    }
    if (tokens.isEmpty()) {
        tokens << "";
    }
    return tokens;
}
