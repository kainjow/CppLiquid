#include "decrement.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::DecrementTag::DecrementTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
    : TagNode(context, tagName, markup)
{
    Parser parser(markup);
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::EndOfString);
}

QString Liquid::DecrementTag::render(Context& context)
{
    Data::Hash& env = context.environments();
    const QString name = to_.toString();
    const int value = env[name].toInt() - 1;
    env[name] = value;
    return QString::number(value);
}


#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::Decrement") {
    
    SECTION("Decrement") {
        CHECK_TEMPLATE_RESULT("{% decrement my_count %} {% decrement my_count %} {% decrement my_count %}", "-1 -2 -3");
        CHECK_TEMPLATE_RESULT("{% assign var = 10 %}{% decrement var %} {% decrement var %} {% decrement var %} {{ var }}", "-1 -2 -3 10");
    }
}

#endif
