#include "case.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::CaseTag::CaseTag(const QStringRef& tagName, const QStringRef& markup)
    : BlockTag(tagName, markup)
{
    Parser parser(markup);
    left_ = Expression::parse(parser);
    (void)parser.consume(Token::Type::EndOfString);
}

void Liquid::CaseTag::parse(Tokenizer& tokenizer)
{
    BlockBody body;
    BlockBody* currentBody = &body;
    while (parseBody(currentBody, tokenizer)) {
        currentBody = &conditions_.back().block();
    }
}

QString Liquid::CaseTag::render(Context& context)
{
    QString output;
    bool executeElseBlock = true;
    const Data leftValue = left_.evaluate(context.data());
    for (auto& cond : conditions_) {
        if (cond.isElse()) {
            if (executeElseBlock) {
                return cond.block().render(context);
            }
        } else if (cond.expression().evaluate(context.data()) == leftValue) {
            executeElseBlock = false;
            output += cond.block().render(context);
        }
    }
    return output;
}

void Liquid::CaseTag::handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer)
{
    if (tagName == "when") {
        BlockBody body;
        Parser parser(markup);
        const Expression condition = Expression::parse(parser);
        // TODO: comma and or separated multiple arguments
        (void)parser.consume(Token::Type::EndOfString);
        conditions_.emplace_back(condition, body);
    } else if (tagName == "else") {
        conditions_.emplace_back(true, BlockBody());
    } else {
        BlockTag::handleUnknownTag(tagName, markup, tokenizer);
    }
}


#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::Case") {
    
    SECTION("Case") {
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 2 %}{% case condition %}{% when 1 %} it's 1 {% when 2 %} it's 2 {% endcase %}",
            " it's 2 "
        );
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 1 %}{% case condition %}{% when 1 %} it's 1 {% when 2 %} it's 2 {% endcase %}",
            " it's 1 "
        );
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 3 %}{% case condition %}{% when 1 %} it's 1 {% when 2 %} it's 2 {% endcase %}",
            ""
        );
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 'string here' %}{% case condition %}{% when 'string here' %} hit {% endcase %}",
            " hit "
        );
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 'bad string here' %}{% case condition %}{% when 'string here' %} hit {% endcase %}",
            ""
        );
    }
    
    SECTION("CaseElse") {
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 5 %}{% case condition %}{% when 5 %} hit {% else %} else {% endcase %}",
            " hit "
        );
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 6 %}{% case condition %}{% when 5 %} hit {% else %} else {% endcase %}",
            " else "
        );
        CHECK_TEMPLATE_RESULT(
            "{% assign condition = 6 %}{% case condition %} {% when 5 %} hit {% else %} else {% endcase %}",
            " else "
        );
    }
}

#endif
