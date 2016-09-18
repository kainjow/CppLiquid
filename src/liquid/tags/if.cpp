#include "if.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::IfTag::IfTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
    : BlockTag(context, tagName, markup)
{
    Parser parser(markup);
    IfBlock block;
    block.cond = parseCondition(parser);
    (void)parser.consume(Token::Type::EndOfString);
    blocks_.push_back(block);
}

void Liquid::IfTag::parse(const Context& context, Tokenizer& tokenizer)
{
    while (parseBody(context, &blocks_.back().body, tokenizer)) {
    }
}

Liquid::Condition Liquid::IfTag::parseCondition(Parser& parser)
{
    const Expression a = Expression::parse(parser);
    if (parser.look(Token::Type::Comparison)) {
        const QStringRef opStr = parser.consume();
        const Expression b = Expression::parse(parser);
        Condition::Operator op = Condition::Operator::None;
        if (opStr == "==") {
            op = Condition::Operator::Equal;
        } else if (opStr == "!=" || opStr == "<>") {
            op = Condition::Operator::NotEqual;
        } else if (opStr == "<") {
            op = Condition::Operator::LessThan;
        } else if (opStr == ">") {
            op = Condition::Operator::GreaterThan;
        } else if (opStr == "<=") {
            op = Condition::Operator::LessOrEqualThan;
        } else if (opStr == ">=") {
            op = Condition::Operator::GreaterOrEqualThan;
        } else if (opStr == "contains") {
            op = Condition::Operator::Contains;
        }
        return Condition(a, op, b);
    }
    return Condition(a);
}

QString Liquid::IfTag::render(Context& context)
{
    for (auto& block : blocks_) {
        if (block.cond.evaluate(context)) {
            return block.body.render(context);
        }
    }
    return "";
}

void Liquid::IfTag::handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer)
{
    if (tagName == "elsif" || tagName == "else") {
    } else {
        BlockTag::handleUnknownTag(tagName, markup, tokenizer);
    }
}

bool Liquid::Condition::evaluate(Context& context)
{
    if (op_ == Operator::None) {
        const Data& result = a_.evaluate(context.data());
        return result.isTruthy();
    }
    throw std::string("Operator not implemented");
}


#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::If") {
    
    SECTION("If") {
        CHECK_TEMPLATE_RESULT(
            " {% if false %} this text should not go into the output {% endif %} ",
            "  "
        );
        CHECK_TEMPLATE_RESULT(
            " {% if true %} this text should go into the output {% endif %} ",
            "  this text should go into the output  "
        );
        CHECK_TEMPLATE_RESULT(
            "{% if false %} falsey {% endif %} {% if true %} truthy {% endif %}?",
            "  truthy ?"
        );
    }

}

#endif
