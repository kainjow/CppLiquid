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
    if (tokens_.empty()) {
        return false;
    }
    token = tokens_[0];
    tokens_.erase(tokens_.cbegin());
    line_number_ += token.count('\n');
    return true;
}

int Liquid::Tokenizer::line_number()
{
    return line_number_;
}

std::vector<QString> Liquid::Tokenizer::tokenize()
{
    return split(source_, QRegularExpression("({%.*?%}|{{.*?}})"));
}

std::vector<QString> Liquid::Tokenizer::split(const QString& source, const QRegularExpression& regex)
{
    Q_ASSERT(regex.isValid());
    std::vector<QString> tokens;
    int pos = 0;
    for (;;) {
        QRegularExpressionMatch match;
        const int find_pos = source.indexOf(regex, pos, &match);
        if (find_pos == -1) {
            break;
        }
        const int diff = find_pos - pos;
        if (diff > 0) {
            tokens.emplace_back(source.mid(pos, diff));
        }
        const QString captured = match.captured();
        tokens.emplace_back(captured);
        pos = find_pos + captured.size();
    }
    if (pos != source.size()) {
        tokens.emplace_back(source.mid(pos));
    }
    if (tokens.empty()) {
        tokens.emplace_back("");
    }
    return tokens;
}
