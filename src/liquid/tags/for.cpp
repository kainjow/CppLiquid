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
    reversed_ = parser.consumeId("reversed");
    while (parser.look(Token::Type::Id)) {
        const QStringRef attr = parser.consume();
        (void)parser.consume(Token::Type::Colon);
        const Expression value = Expression::parse(parser);
        if (attr == "offset") {
            offset_ = value;
        } else if (attr == "limit") {
            limit_ = value;
        } else {
            throw std::string("Invalid attribute in for loop. Valid attributes are limit and offset");
        }
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
    QString output;
    Data& data = context.data();
    const QString varName = varName_.toString();

    if (range_) {
        const int start = rangeStart_.evaluate(data).toInt();
        const int end = rangeEnd_.evaluate(data).toInt();
        const bool empty = end < start;
        if (empty) {
            return elseBlock_.render(context);
        }
        if (reversed_) {
            for (int i = end; i >= start; --i) {
                data.insert(varName, i);
                output += body_.render(context);
            }
        } else {
            for (int i = start; i <= end; ++i) {
                data.insert(varName, i);
                output += body_.render(context);
            }
        }
    } else {
        const Data& collection = collection_.evaluate(data);
        const int start = 0;
        const int end = static_cast<int>(collection.size()) - 1;
        const bool empty = (!collection.isArray() && collection.isHash()) || collection.size() == 0;
        if (empty) {
            return elseBlock_.render(context);
        }
        if (reversed_) {
            for (int i = end; i >= start; --i) {
                data.insert(varName, collection.at(static_cast<size_t>(i)));
                output += body_.render(context);
            }
        } else {
            for (int i = start; i <= end; ++i) {
                data.insert(varName, collection.at(static_cast<size_t>(i)));
                output += body_.render(context);
            }
        }
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

    SECTION("ForRange") {
        CHECK_TEMPLATE_RESULT(
            "{%for i in (1..2) %}{% assign a = 'variable'%}{% endfor %}{{a}}",
            "variable"
        );
        CHECK_TEMPLATE_RESULT(
            "{%for item in (1..3) %} {{item}} {%endfor%}",
            " 1  2  3 "
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in (1..foobar) %} {{item}} {%endfor%}",
            " 1  2  3 ",
            (Liquid::Data::Hash{{"foobar", 3}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in (1..foobar.value) %} {{item}} {%endfor%}",
            " 1  2  3 ",
            (Liquid::Data::Hash{{"foobar", Liquid::Data::Hash{{"value", 3}}}})
        );
    }
    
    SECTION("ForReversed") {
        CHECK_TEMPLATE_RESULT(
            "{%for item in (1..3) reversed %} {{item}} {%endfor%}",
            " 3  2  1 "
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed %}{{item}}{%endfor%}",
            "321",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
    }
    
    SECTION("ForVariable") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{item}} {%endfor%}",
            " 1  2  3 ",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "123",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% for item in array %}{{item}}{% endfor %}",
            "123",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "abcd",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{"a", "b", "c", "d"}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "a b c",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{"a", " ", "b", " ", "c"}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "abc",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{"a", "", "b", "", "c"}}})
        );
    }

}

#endif
