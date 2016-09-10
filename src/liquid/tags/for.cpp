#include "for.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::ForTag::ForTag(const QStringRef& tagName, const QStringRef& markup)
    : BlockTag(tagName, markup)
{
    Parser parser(markup);
    varName_ = parser.consume(Token::Type::Id);
    if (parser.consume(Token::Type::Id) != "in") {
        throw std::string("Syntax Error in 'for loop' - Valid syntax: for [item] in [collection]");
    }
    if (parser.look(Token::Type::OpenRound)) {
        (void)parser.consume();
        rangeStart_ = Expression::parse(parser);
        (void)parser.consume(Token::Type::DotDot);
        rangeEnd_ = Expression::parse(parser);
        (void)parser.consume(Token::Type::CloseRound);
    }
    (void)parser.consume(Token::Type::EndOfString);
}

void Liquid::ForTag::parse(Tokenizer& tokenizer)
{
    if (!parseBody(&body_, tokenizer)) {
        return;
    }
    (void)parseBody(&elseBlock_, tokenizer);
}

QString Liquid::ForTag::render(Context& context)
{
    const int start = rangeStart_.evaluate(context.data()).toInt();
    const int end = rangeEnd_.evaluate(context.data()).toInt();
    const bool empty = end < start;
    if (empty) {
        return elseBlock_.render(context);
    }
    QString output;
    Data& data = context.data();
    const QString varName = varName_.toString();
    for (int i = start; i <= end; ++i) {
        data.insert(varName, i);
        output += body_.render(context);
    }
    return output;
}

void Liquid::ForTag::handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer)
{
    (void)tagName;
    (void)markup;
    (void)tokenizer;
}


#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::For") {
    
    SECTION("For") {
        CHECK_TEMPLATE_RESULT(
            "{%for i in (1..2) %}{% assign a = 'variable'%}{% endfor %}{{a}}",
            "variable"
        );
    }
}

#endif
