#include "assign.hpp"
#include "context.hpp"
#include "template.hpp"
#include <QDebug>

Liquid::AssignTag::AssignTag(Parser& parser)
{
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

#include "catch.hpp"

TEST_CASE("Liquid::Assign") {
    
    SECTION("Assign") {
        Liquid::Template t;
        CHECK(t.parse("{% assign name = 'Steve' %}{{ name }}").render().toStdString() == "Steve");
    }
    
    SECTION("Hyphenated") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["a-b"] = "1";
        Liquid::Data data(hash);
        CHECK(t.parse("a-b:{{a-b}} {%assign a-b = 2 %}a-b:{{a-b}}").render(data) == "a-b:1 a-b:2");
    }
}

#endif
