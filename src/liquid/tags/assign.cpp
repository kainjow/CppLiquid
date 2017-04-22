#include "assign.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::AssignTag::AssignTag(const Context& context, const StringRef& tagName, const StringRef& markup)
    : TagNode(context, tagName, markup)
{
    Parser parser(markup);
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::Equal);
    from_ = Variable(parser);
}
    
Liquid::String Liquid::AssignTag::render(Context& ctx)
{
    Data& data = ctx.data();
    data.insert(to_.toString(), from_.evaluate(ctx));
    return "";
}



#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::Assign") {
    
    SECTION("Assign") {
        CHECK_TEMPLATE_RESULT("{% assign name = 'Steve' %}{{ name }}", "Steve");
        CHECK_TEMPLATE_RESULT("{% assign a = ""%}{{a}}", "");
        CHECK_TEMPLATE_DATA_RESULT(
            "var2:{{var2}} {%assign var2 = var%} var2:{{var2}}",
            "var2:  var2:content",
            (Liquid::Data::Hash{{"var", "content"}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "a-b:{{a-b}} {%assign a-b = 2 %}a-b:{{a-b}}",
            "a-b:1 a-b:2",
            (Liquid::Data::Hash{{"a-b", "1"}})
        );
        CHECK_TEMPLATE_RESULT("{% assign age = 32 | plus: 4 | divided_by: 2 %}{{ age }}", "18");
        CHECK_TEMPLATE_RESULT("{% assign x = 1 %}{% assign x = x | plus: 1 %}{{ x }}", "2");
    }
}

#endif
