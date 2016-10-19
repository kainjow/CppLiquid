#include "increment.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::IncrementTag::IncrementTag(const Context& context, const StringRef& tagName, const StringRef& markup)
    : TagNode(context, tagName, markup)
{
    Parser parser(markup);
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::EndOfString);
}

Liquid::String Liquid::IncrementTag::render(Context& context)
{
    Data::Hash& env = context.environments();
    const String name = to_.toString();
    const int value = env[name].toInt();
    env[name] = value + 1;
    return String(std::to_string(value));
}



#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::Increment") {
    
    SECTION("Increment") {
        CHECK_TEMPLATE_RESULT("{% increment my_count %} {% increment my_count %} {% increment my_count %}", "0 1 2");
        CHECK_TEMPLATE_RESULT("{% assign var = 10 %}{% increment var %} {% increment var %} {% increment var %} {{ var }}", "0 1 2 10");
    }
}

#endif
