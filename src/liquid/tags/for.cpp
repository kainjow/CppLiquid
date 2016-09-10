#include "for.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::ForTag::ForTag(const QStringRef& tagName, const QStringRef& markup)
    : BlockTag(tagName, markup)
    , range_(false)
{
    Parser parser(markup);
    varName_ = parser.consume(Token::Type::Id);
    if (parser.consume(Token::Type::Id) != "in") {
        throw std::string("Syntax Error in 'for loop' - Valid syntax: for [item] in [collection]");
    }
    if (parser.look(Token::Type::OpenRound)) {
        range_ = true;
        (void)parser.consume();
        rangeStart_ = Expression::parse(parser);
        (void)parser.consume(Token::Type::DotDot);
        rangeEnd_ = Expression::parse(parser);
        (void)parser.consume(Token::Type::CloseRound);
    } else {
        collection_ = Expression::parse(parser);
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
    Data& data = context.data();

    if (range_) {
        const int start = rangeStart_.evaluate(data).toInt();
        const int end = rangeEnd_.evaluate(data).toInt();
        const bool empty = end < start;
        if (empty) {
            return elseBlock_.render(context);
        }
        QString output;
        const QString varName = varName_.toString();
        for (int i = start; i <= end; ++i) {
            data.insert(varName, i);
            output += body_.render(context);
        }
        return output;
    }
    
    const Data& collection = collection_.evaluate(data);
    const bool empty = (!collection.isArray() && collection.isHash()) || collection.size() == 0;
    if (empty) {
        return elseBlock_.render(context);
    }
    const int start = 0;
    const int end = static_cast<int>(collection.size()) - 1;
    QString output;
    const QString varName = varName_.toString();
    for (int i = start; i <= end; ++i) {
        data.insert(varName, collection.at(static_cast<size_t>(i)));
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
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} yo {%endfor%}",
            " yo  yo  yo  yo ",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3, 4}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}yo{%endfor%}",
            "yoyo",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} yo {%endfor%}",
            " yo ",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{%endfor%}",
            "",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2}}})
        );
    }
}

#endif
