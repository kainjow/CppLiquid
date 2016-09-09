#include "cycle.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::CycleTag::CycleTag(const QStringRef& tagName, const QStringRef& markup)
    : TagNode(tagName, markup)
{
    Parser parser(markup);
    expressions_.push_back(Expression::parse(parser));
    while (parser.look(Token::Type::Comma)) {
        (void)parser.consume(Token::Type::Comma);
        expressions_.push_back(Expression::parse(parser));
    }
    (void)parser.consume(Token::Type::EndOfString);
    for (const auto& expression : expressions_) {
        name_ += expression.stringDescription();
    }
}

QString Liquid::CycleTag::render(Context& context)
{
    Data::Hash& registers = context.registers();
    const QString name = tagName().toString();
    if (registers.find(name) == registers.end()) {
        registers[name] = Data::Hash();
    }
    int iteration = 0;
    Data& reg = registers[name];
    if (reg.containsKey(name_)) {
        iteration = reg[name_].toInt();
    }
    const QString result = expressions_[iteration].evaluate(context.data()).toString();
    ++iteration;
    if (iteration >= static_cast<int>(expressions_.size())) {
        iteration = 0;
    }
    reg.insert(name_, iteration);
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
    }
}

#endif
