#include "assign.hpp"
#include "context.hpp"
#include "template.hpp"
#include <QDebug>

Liquid::AssignTag::AssignTag(const QStringRef& tagName, const QStringRef& markup)
    : TagNode(tagName, markup)
{
    Parser parser(markup);
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::Equal);
    from_ = Variable(parser);
}
    
QString Liquid::AssignTag::render(Context& ctx)
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
    }
    
    SECTION("Hyphenated") {
        CHECK_TEMPLATE_DATA_RESULT(
            "a-b:{{a-b}} {%assign a-b = 2 %}a-b:{{a-b}}",
            "a-b:1 a-b:2",
            (Liquid::Data::Hash{{"a-b", "1"}})
        );
    }

    SECTION("Filter") {
        CHECK_TEMPLATE_RESULT("{% assign age = 32 | plus: 4 | divided_by: 2 %}{{ age }}", "18");
    }
}

#endif
