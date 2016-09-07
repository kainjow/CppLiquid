#include "increment.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::IncrementTag::IncrementTag(const QStringRef& tagName, Parser& parser)
    : TagNode(tagName, parser)
{
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::EndOfString);
}

QString Liquid::IncrementTag::render(Context& context)
{
    Data& env = context.environments();
    const QString name = to_.toString();
    const int value = env[name].toInt();
    env.insert(name, value + 1);
    return QString::number(value);
}

Liquid::DecrementTag::DecrementTag(const QStringRef& tagName, Parser& parser)
    : TagNode(tagName, parser)
{
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::EndOfString);
}

QString Liquid::DecrementTag::render(Context& context)
{
    Data& env = context.environments();
    const QString name = to_.toString();
    const int value = env[name].toInt() - 1;
    env.insert(name, value);
    return QString::number(value);
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Increment") {
    
    SECTION("Increment") {
        Liquid::Template t;
        CHECK(t.parse("{% increment my_count %} {% increment my_count %} {% increment my_count %}").render().toStdString() == "0 1 2");
        CHECK(t.parse("{% assign var = 10 %}{% increment var %} {% increment var %} {% increment var %} {{ var }}").render().toStdString() == "0 1 2 10");
    }
}

TEST_CASE("Liquid::Decrement") {
    
    SECTION("Decrement") {
        Liquid::Template t;
        CHECK(t.parse("{% decrement my_count %} {% decrement my_count %} {% decrement my_count %}").render().toStdString() == "-1 -2 -3");
        CHECK(t.parse("{% assign var = 10 %}{% decrement var %} {% decrement var %} {% decrement var %} {{ var }}").render().toStdString() == "-1 -2 -3 10");
    }
}

#endif
