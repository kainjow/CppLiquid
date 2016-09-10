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
    BlockBody newBody;
    BlockBody* currentBody = &newBody;
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
        BlockBody body;
        body.parse(tokenizer);
        conditions_.emplace_back(true, body);
    } else {
        BlockTag::handleUnknownTag(tagName, markup, tokenizer);
    }
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Case") {
    
    SECTION("CaseTag") {
        Liquid::Template t;
        CHECK(t.parse("{% assign condition = 2 %}{% case condition %}{% when 1 %} it's 1 {% when 2 %} it's 2 {% endcase %}").render().toStdString() == " it's 2 ");
    }
}

#endif
