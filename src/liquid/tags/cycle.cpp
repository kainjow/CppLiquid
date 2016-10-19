#include "cycle.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::CycleTag::CycleTag(const Context& context, const StringRef& tagName, const StringRef& markup)
    : TagNode(context, tagName, markup)
{
    Parser parser(markup);
    const Expression firstExpression = Expression::parse(parser);
    if (parser.look(Token::Type::Colon)) {
        (void)parser.consume(Token::Type::Colon);
        nameExpression_ = firstExpression;
        expressions_.push_back(Expression::parse(parser));
        nameIsExpression_ = true;
    } else {
        expressions_.push_back(firstExpression);
        nameIsExpression_ = false;
    }
    while (parser.look(Token::Type::Comma)) {
        (void)parser.consume(Token::Type::Comma);
        expressions_.push_back(Expression::parse(parser));
    }
    (void)parser.consume(Token::Type::EndOfString);
    if (!nameIsExpression_) {
        for (const auto& expression : expressions_) {
            nameString_ += expression.stringDescription();
        }
    }
}

Liquid::String Liquid::CycleTag::render(Context& context)
{
    Data::Hash& registers = context.registers();
    const String name = tagName().toString();
    if (registers.find(name) == registers.end()) {
        registers[name] = Data::Hash();
    }
    int iteration = 0;
    Data& reg = registers[name];
    const String lookupKey = nameIsExpression_ ? nameExpression_.evaluate(context.data()).toString() : nameString_;
    if (reg.containsKey(lookupKey)) {
        iteration = reg[lookupKey].toInt();
    }
    const String result = expressions_[iteration].evaluate(context.data()).toString();
    ++iteration;
    if (iteration >= static_cast<int>(expressions_.size())) {
        iteration = 0;
    }
    reg.insert(lookupKey, iteration);
    return result;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Cycle") {
    
    SECTION("Cycle") {
        Liquid::Template t;
        CHECK(t.parse("{%cycle 'one', 'two'%}").render().toStdString() == "one");
        CHECK(t.parse("{%cycle 'one', 'two'%} {%cycle 'one', 'two'%}").render().toStdString() == "one two");
        CHECK(t.parse("{%cycle '', 'two'%} {%cycle '', 'two'%}").render().toStdString() == " two");
        CHECK(t.parse("{%cycle 'one', 'two'%} {%cycle 'one', 'two'%} {%cycle 'one', 'two'%}").render().toStdString() == "one two one");
        CHECK(t.parse("{%cycle 'text-align: left', 'text-align: right' %} {%cycle 'text-align: left', 'text-align: right'%}").render().toStdString() == "text-align: left text-align: right");
        CHECK(t.parse("{%cycle 1,2%} {%cycle 1,2%} {%cycle 1,2%} {%cycle 1,2,3%} {%cycle 1,2,3%} {%cycle 1,2,3%} {%cycle 1,2,3%}").render().toStdString() == "1 2 1 1 2 3 1");
        CHECK(t.parse("{%cycle 1: 'one', 'two' %} {%cycle 2: 'one', 'two' %} {%cycle 1: 'one', 'two' %} {%cycle 2: 'one', 'two' %} {%cycle 1: 'one', 'two' %} {%cycle 2: 'one', 'two' %}").render().toStdString() == "one one two two one one");
        Liquid::Data::Hash hash;
        hash["var1"] = 1;
        hash["var2"] = 2;
        Liquid::Data data(hash);
        CHECK(t.parse("{%cycle var1: 'one', 'two' %} {%cycle var2: 'one', 'two' %} {%cycle var1: 'one', 'two' %} {%cycle var2: 'one', 'two' %} {%cycle var1: 'one', 'two' %} {%cycle var2: 'one', 'two' %}").render(data).toStdString() == "one one two two one one");
    }
}

#endif
