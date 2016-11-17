#include "if.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::IfTag::IfTag(bool unless, const Context& context, const StringRef& tagName, const StringRef& markup)
    : BlockTag(context, tagName, markup)
    , if_(!unless)
{
    blocks_.emplace_back(false);
    parseTag(markup);
}

void Liquid::IfTag::parseTag(const StringRef& markup)
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
        const StringRef opStr = parser.consume();
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

Liquid::String Liquid::IfTag::render(Context& context)
{
    for (auto& block : blocks_) {
        const bool result = block.cond.evaluate(context);
        if (block.isElse || (if_ && result) || (!if_ && !result)) {
            return block.body.render(context);
        }
    }
    return "";
}

void Liquid::IfTag::handleUnknownTag(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer)
{
    if (tagName == "elsif") {
        blocks_.emplace_back(false);
        parseTag(markup);
    } else if (tagName == "else") {
        blocks_.emplace_back(true);
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
        case Operator::NotEqual:
            result = v1 != v2;
            break;
        case Operator::LessThan:
            result = v1 < v2;
            break;
        case Operator::LessOrEqualThan:
            result = v1 <= v2;
            break;
        case Operator::GreaterThan:
            result = v1 > v2;
            break;
        case Operator::GreaterOrEqualThan:
            result = v1 >= v2;
            break;
        case Operator::Contains:
            if (v1.isArray()) {
                result = std::find(v1.array().cbegin(), v1.array().cend(), v2) != v1.array().cend();
            } else if (v1.isHash()) {
                result = v1.hash().find(v2.toString()) != v1.hash().end();
            } else if (v1.isString()) {
                result = v1.toString().indexOf(v2.toString()) != String::npos;
            } else {
                result = false;
            }
            break;
        default:
            throw std::runtime_error("Operator not implemented");
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

    SECTION("IfOrOperators") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a == true or b == true %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"a", true}, {"b", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a == true or b == false %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"a", true}, {"b", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a == false or b == false %} YES {% endif %}",
            "",
            (Liquid::Data::Hash{{"a", true}, {"b", true}})
        );
    }

    SECTION("IfComparisonOfStringsContainingAndOrOr") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if a == 'and' and b == 'or' and c == 'foo and bar' and d == 'bar or baz' and e == 'foo' and foo and bar %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"a", "and"}, {"b", "or"}, {"c", "foo and bar"}, {"d", "bar or baz"}, {"e", "foo"}, {"foo", true}, {"bar", true}})
        );
    }

    SECTION("IfComparisonOfExpressionsStartingWithAndOrOr") {
        const Liquid::Data::Hash data = Liquid::Data::Hash{
            {"order", Liquid::Data::Hash{{"items_count", 0}}},
            {"android", Liquid::Data::Hash{{"name", "Roy"}}}
        };
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if android.name == 'Roy' %}YES{% endif %}",
            "YES",
            data
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if order.items_count == 0 %}YES{% endif %}",
            "YES",
            data
        );
    }

    SECTION("IfAnd") {
        CHECK_TEMPLATE_RESULT(
            "{% if true and true %} YES {% endif %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% if false and true %} YES {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if false and true %} YES {% endif %}",
            ""
        );
    }

    SECTION("IfHashMissGeneratesFalse") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% endif %}",
            "",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{}}})
        );
    }

    SECTION("IfFromVariable") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} NO {% endif %}",
            "",
            (Liquid::Data::Hash{{"var", false}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} NO {% endif %}",
            "",
            (Liquid::Data::Hash{{"var", nullptr}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% endif %}",
            "",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", false}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% endif %}",
            "",
            (Liquid::Data::Hash{{"foo", {}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% endif %}",
            "",
            (Liquid::Data::Hash{{"foo", nullptr}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% endif %}",
            "",
            (Liquid::Data::Hash{{"foo", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", "text"}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", 1}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", Liquid::Data::Hash{}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", Liquid::Data::Array{}}})
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 'foo' %} YES {% endif %}",
            " YES "
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", true}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", "text"}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", 1}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", Liquid::Data::Hash{}}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", Liquid::Data::Array{}}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} NO {% else %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", false}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} NO {% else %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", nullptr}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if var %} YES {% else %} NO {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", true}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if 'foo' %} YES {% else %} NO {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"var", "text"}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% else %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", false}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} YES {% else %} NO {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", true}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} YES {% else %} NO {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"bar", "text"}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% else %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{{"notbar", true}}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% else %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"foo", Liquid::Data::Hash{}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% if foo.bar %} NO {% else %} YES {% endif %}",
            " YES ",
            (Liquid::Data::Hash{{"notfoo", Liquid::Data::Hash{{"bar", true}}}})
        );
    }

    SECTION("IfNested") {
        CHECK_TEMPLATE_RESULT(
            "{% if false %}{% if false %} NO {% endif %}{% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if false %}{% if true %} NO {% endif %}{% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if true %}{% if false %} NO {% endif %}{% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if true %}{% if true %} YES {% endif %}{% endif %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% if true %}{% if true %} YES {% else %} NO {% endif %}{% else %} NO {% endif %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% if true %}{% if false %} NO {% else %} YES {% endif %}{% else %} NO {% endif %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% if false %}{% if true %} NO {% else %} NONO {% endif %}{% else %} YES {% endif %}",
            " YES "
        );
    }

    SECTION("IfComparisonsOnNull") {
        CHECK_TEMPLATE_RESULT(
            "{% if null < 10 %} NO {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if null <= 10 %} NO {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if null >= 10 %} NO {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if null > 10 %} NO {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 10 < null %} NO {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 10 <= null %} NO {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 10 >= null %} NO {% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 10 > null %} NO {% endif %}",
            ""
        );
    }

    SECTION("IfElseIf") {
        CHECK_TEMPLATE_RESULT(
            "{% if 0 == 0 %}0{% elsif 1 == 1%}1{% else %}2{% endif %}",
            "0"
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 0 != 0 %}0{% elsif 1 == 1%}1{% else %}2{% endif %}",
            "1"
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 0 != 0 %}0{% elsif 1 != 1%}1{% else %}2{% endif %}",
            "2"
        );
        CHECK_TEMPLATE_RESULT(
            "{% if false %}if{% elsif true %}elsif{% endif %}",
            "elsif"
        );
    }
    
    SECTION("IfContains") {
        CHECK_TEMPLATE_RESULT(
            "{% if 'bob' contains 'o' %}yes{% endif %}",
            "yes"
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 'bob' contains 'f' %}yes{% else %}no{% endif %}",
            "no"
        );
        CHECK_TEMPLATE_RESULT(
            "{% if 'gnomeslab-and-or-liquid' contains 'gnomeslab-and-or-liquid' %}yes{% endif %}",
            "yes"
        );
    }
    
    SECTION("IfMultipleConditions") {
        const std::vector<std::tuple<bool, bool, bool, std::string>> tests{
            {true, true, true, "true"},
            {true, true, false, "true"},
            {true, false, true, "true"},
            {true, false, false, "true"},
            {false, true, true, "true"},
            {false, true, false, "false"},
            {false, false, true, "false"},
            {false, false, false, "false"},
        };
        for (const auto& test : tests) {
            CHECK_TEMPLATE_DATA_RESULT(
                "{% if a or b and c %}true{% else %}false{% endif %}",
                std::get<3>(test),
                (Liquid::Data::Hash{{"a", std::get<0>(test)}, {"b", std::get<1>(test)}, {"c", std::get<2>(test)}})
            );
        }
    }

    SECTION("IfIllegalSymbols") {
        CHECK_TEMPLATE_RESULT(
            "{% if true == empty %}?{% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if true == null %}?{% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if empty == true %}?{% endif %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% if null == true %}?{% endif %}",
            ""
        );
    }
    
    SECTION("Unless") {
        CHECK_TEMPLATE_RESULT(
            " {% unless true %} this text should not go into the output {% endunless %} ",
            "  "
        );
        CHECK_TEMPLATE_RESULT(
            " {% unless false %} this text should go into the output {% endunless %} ",
            "  this text should go into the output  "
        );
        CHECK_TEMPLATE_RESULT(
            "{% unless true %} you suck {% endunless %} {% unless false %} you rock {% endunless %}?",
            "  you rock ?"
        );        CHECK_TEMPLATE_RESULT(
            "{% unless true %} NO {% else %} YES {% endunless %}",
            " YES "
        );

    }

    SECTION("UnlessElse") {
        CHECK_TEMPLATE_RESULT(
            "{% unless true %} NO {% else %} YES {% endunless %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% unless false %} YES {% else %} NO {% endunless %}",
            " YES "
        );
        CHECK_TEMPLATE_RESULT(
            "{% unless 'foo' %} NO {% else %} YES {% endunless %}",
            " YES "
        );
    }

    SECTION("UnlessInLoop") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% for i in choices %}{% unless i %}{{ forloop.index }}{% endunless %}{% endfor %}",
            "23",
            (Liquid::Data::Hash{{"choices", Liquid::Data::Array{1, nullptr, false}}})
        );
    }

    SECTION("UnlessElseInLoop") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{% for i in choices %}{% unless i %} {{ forloop.index }} {% else %} TRUE {% endunless %}{% endfor %}",
            " TRUE  2  3 ",
            (Liquid::Data::Hash{{"choices", Liquid::Data::Array{1, nullptr, false}}})
        );
    }
}

#endif
