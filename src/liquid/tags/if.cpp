#include "if.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::IfTag::IfTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
    : BlockTag(context, tagName, markup)
{
    IfBlock block;
    blocks_.push_back(block);
    parseTag(markup);
}

void Liquid::IfTag::parseTag(const QStringRef& markup)
{
    Parser parser(markup);
    blocks_.back().cond = parseLogicalCondition(parser);
    (void)parser.consume(Token::Type::EndOfString);
}

void Liquid::IfTag::parse(const Context& context, Tokenizer& tokenizer)
{
    while (parseBody(context, &blocks_.back().body, tokenizer)) {
    }
}

Liquid::Condition Liquid::IfTag::parseLogicalCondition(Parser& parser)
{
    Condition cond = parseCondition(parser);
    Condition::LogicalOperator logicalOp = Condition::LogicalOperator::None;
    if (parser.consumeId("and")) {
        logicalOp = Condition::LogicalOperator::And;
    } else if (parser.consumeId("or")) {
        logicalOp = Condition::LogicalOperator::Or;
    }
    if (logicalOp != Condition::LogicalOperator::None) {
        cond.setLogicalCondition(logicalOp, std::make_shared<Condition>(parseLogicalCondition(parser)));
    }
    return cond;
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
        if (block.isElse || block.cond.evaluate(context)) {
            return block.body.render(context);
        }
    }
    return "";
}

void Liquid::IfTag::handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer)
{
    if (tagName == "elsif") {
        IfBlock block;
        blocks_.push_back(block);
        parseTag(markup);
    } else if (tagName == "else") {
        IfBlock block;
        block.isElse = true;
        blocks_.push_back(block);
    } else {
        BlockTag::handleUnknownTag(tagName, markup, tokenizer);
    }
}

bool Liquid::Condition::evaluate(Context& context)
{
    bool result;
    Data& data = context.data();
    const Data& v1 = a_.evaluate(data);
    const Data& v2 = b_.evaluate(data);
    switch (op_) {
        case Operator::None:
            result = v1.isTruthy();
            break;
        case Operator::Equal:
            result = v1 == v2;
            break;
        default:
            throw std::string("Operator not implemented");
    }
    switch (logicalOp_) {
        case LogicalOperator::And:
            return result && child_->evaluate(context);
        case LogicalOperator::Or:
            return result || child_->evaluate(context);
        default:
            break;
    }
    return result;
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

    SECTION("IfLiteralComparisons") {
        CHECK_TEMPLATE_RESULT(
            "{% assign v = false %}{% if v %} YES {% else %} NO {% endif %}",
            " NO "
        );
        CHECK_TEMPLATE_RESULT(
            "{% assign v = nil %}{% if v == nil %} YES {% else %} NO {% endif %}",
            " YES "
        );
    }

    SECTION("IfElse") {
        CHECK_TEMPLATE_RESULT(
            "{% if false %} NO {% else %} YES {% endif %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% if true %} YES {% else %} NO {% endif %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 'foo' %} YES {% else %} NO {% endif %}",
            " YES "
        );
    }

    SECTION("IfBoolean") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", true}})
        );
    }

    SECTION("IfOr") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a or b %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"a", true}, {"b", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a or b %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"a", true}, {"b", false}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a or b %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"a", false}, {"b", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a or b %} YES {% endif %}",
            "",
            (Liquid::Data::Hash{{"a", false}, {"b", false}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a or b or c %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"a", false}, {"b", false}, {"c", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a or b or c %} YES {% endif %}",
            "",
            (Liquid::Data::Hash{{"a", false}, {"b", false}, {"c", false}})
        );
    }
}

#endif
