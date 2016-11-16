#include "case.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"
#include "error.hpp"

Liquid::CaseTag::CaseTag(const Context& context, const StringRef& tagName, const StringRef& markup)
    : BlockTag(context, tagName, markup)
{
    Parser parser(markup);
    left_ = Expression::parse(parser);
    (void)parser.consume(Token::Type::EndOfString);
}

void Liquid::CaseTag::parse(const Context& context, Tokenizer& tokenizer)
{
    BlockBody body;
    BlockBody* currentBody = &body;
    while (parseBody(context, currentBody, tokenizer)) {
        currentBody = &conditions_.back().block();
    }
}

Liquid::String Liquid::CaseTag::render(Context& context)
{
    String output;
    bool executeElseBlock = true;
    const Data leftValue = left_.evaluate(context.data());
    for (auto& cond : conditions_) {
        if (cond.isElse()) {
            if (executeElseBlock) {
                return cond.block().render(context);
            }
        } else {
            for (const auto& exp : cond.expressions()) {
                if (exp.evaluate(context.data()) == leftValue) {
                    executeElseBlock = false;
                    output += cond.block().render(context);
                }
            }
        }
    }
    return output;
}

void Liquid::CaseTag::handleUnknownTag(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer)
{
    if (tagName == "when") {
        BlockBody body;
        Parser parser(markup);
        std::vector<Expression> expressions;
        expressions.push_back(Expression::parse(parser));
        if (parser.look(Token::Type::Id)) {
            while (parser.look(Token::Type::Id)) {
                const StringRef orId = parser.consume();
                if (orId != "or") {
                    throw syntax_error(String("Expected \"or\" but found %1").arg(orId.toString()));
                }
                expressions.push_back(Expression::parse(parser));
            }
        } else if (parser.look(Token::Type::Comma)) {
            while (parser.look(Token::Type::Comma)) {
                (void)parser.consume();
                expressions.push_back(Expression::parse(parser));
            }
        }
        (void)parser.consume(Token::Type::EndOfString);
        conditions_.emplace_back(expressions, body);
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
    
    SECTION("CaseAssign") {
        const Liquid::String code = "{% case collection.handle %}{% when 'menswear-jackets' %}{% assign ptitle = 'menswear' %}{% when 'menswear-t-shirts' %}{% assign ptitle = 'menswear' %}{% else %}{% assign ptitle = 'womenswear' %}{% endcase %}{{ ptitle }}";
        Liquid::Template t;
        t.parse(code);
        CHECK_DATA_RESULT(t, "menswear",
            (Liquid::Data::Hash{{"collection", Liquid::Data::Hash{{"handle", "menswear-jackets"}}}})
        );
        CHECK_DATA_RESULT(t, "menswear",
            (Liquid::Data::Hash{{"collection", Liquid::Data::Hash{{"handle", "menswear-t-shirts"}}}})
        );
        CHECK_DATA_RESULT(t, "womenswear",
            (Liquid::Data::Hash{{"collection", Liquid::Data::Hash{{"handle", "x"}}}})
        );
        CHECK_DATA_RESULT(t, "womenswear",
            (Liquid::Data::Hash{{"collection", Liquid::Data::Hash{{"handle", "y"}}}})
        );
        CHECK_DATA_RESULT(t, "womenswear",
            (Liquid::Data::Hash{{"collection", Liquid::Data::Hash{{"handle", "z"}}}})
        );
    }
    
    SECTION("CaseOr1") {
        const Liquid::String code = "{% case condition %}{% when 1 or 2 or 3 %} it's 1 or 2 or 3 {% when 4 %} it's 4 {% endcase %}";
        Liquid::Template t;
        t.parse(code);
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 1}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 2}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 3}}));
        CHECK_DATA_RESULT(t, " it's 4 ", (Liquid::Data::Hash{{"condition", 4}}));
        CHECK_DATA_RESULT(t, "", (Liquid::Data::Hash{{"condition", 5}}));
    }

    SECTION("CaseOr2") {
        const Liquid::String code = "{% case condition %}{% when 1 or 'string' or null %} it's 1 or 2 or 3 {% when 4 %} it's 4 {% endcase %}";
        Liquid::Template t;
        t.parse(code);
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 1}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", "string"}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", nullptr}}));
        CHECK_DATA_RESULT(t, "", (Liquid::Data::Hash{{"condition", "something else"}}));
    }
    
    SECTION("CaseComma1") {
        const Liquid::String code = "{% case condition %}{% when 1, 2, 3 %} it's 1 or 2 or 3 {% when 4 %} it's 4 {% endcase %}";
        Liquid::Template t;
        t.parse(code);
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 1}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 2}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 3}}));
        CHECK_DATA_RESULT(t, " it's 4 ", (Liquid::Data::Hash{{"condition", 4}}));
        CHECK_DATA_RESULT(t, "", (Liquid::Data::Hash{{"condition", 5}}));
    }
    
    SECTION("CaseComma2") {
        const Liquid::String code = "{% case condition %}{% when 1, 'string', null %} it's 1 or 2 or 3 {% when 4 %} it's 4 {% endcase %}";
        Liquid::Template t;
        t.parse(code);
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", 1}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", "string"}}));
        CHECK_DATA_RESULT(t, " it's 1 or 2 or 3 ", (Liquid::Data::Hash{{"condition", nullptr}}));
        CHECK_DATA_RESULT(t, "", (Liquid::Data::Hash{{"condition", "something else"}}));
    }
}

#endif
