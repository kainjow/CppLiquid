#include "for.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::ForTag::ForTag(const QStringRef& tagName, const QStringRef& markup)
    : BlockTag(tagName, markup)
    , range_(false)
{
    Parser parser(markup);
    varName_ = parser.consume(Token::Type::Id);
    if (parser.consume(Token::Type::Id) != "in") {
        throw std::string("Syntax Error in 'for loop' - Valid syntax: for [item] in [collection]");
    }
    if (parser.look(Token::Type::OpenRound)) {
        range_ = true;
        (void)parser.consume();
        rangeStart_ = Expression::parse(parser);
        (void)parser.consume(Token::Type::DotDot);
        rangeEnd_ = Expression::parse(parser);
        (void)parser.consume(Token::Type::CloseRound);
    } else {
        collection_ = Expression::parse(parser);
    }
    reversed_ = parser.consumeId("reversed");
    while (parser.look(Token::Type::Id)) {
        const QStringRef attr = parser.consume();
        (void)parser.consume(Token::Type::Colon);
        const Expression value = Expression::parse(parser);
        if (attr == "offset") {
            offset_ = value;
        } else if (attr == "limit") {
            limit_ = value;
        } else {
            throw std::string("Invalid attribute in for loop. Valid attributes are limit and offset");
        }
    }
    (void)parser.consume(Token::Type::EndOfString);
}

void Liquid::ForTag::parse(Tokenizer& tokenizer)
{
    if (!parseBody(&body_, tokenizer)) {
        return;
    }
    (void)parseBody(&elseBlock_, tokenizer);
}

QString Liquid::ForTag::render(Context& context)
{
    QString output;
    Data& data = context.data();
    const QString varName = varName_.toString();

    if (range_) {
        const int start = rangeStart_.evaluate(data).toInt();
        const int end = rangeEnd_.evaluate(data).toInt();
        const bool empty = end < start;
        if (empty) {
            return elseBlock_.render(context);
        }
        if (reversed_) {
            for (int i = end; i >= start; --i) {
                data.insert(varName, i);
                output += body_.render(context);
            }
        } else {
            for (int i = start; i <= end; ++i) {
                data.insert(varName, i);
                output += body_.render(context);
            }
        }
    } else {
        const Data& collection = collection_.evaluate(data);
        const Data limitDat = limit_.evaluate(data);
        const Data offsetDat = offset_.evaluate(data);
        const int initialStart = 0;
        const int initialEnd = static_cast<int>(collection.size()) - 1;
        const int start = offsetDat.isNumber() ? offsetDat.toInt() : initialStart;
        const int end = limitDat.isNumber() ? start + (limitDat.toInt() - 1) : initialEnd;
        const bool empty = (!collection.isArray() && collection.isHash()) || collection.size() == 0;
        const int len = (end - start) + 1;
        if (empty) {
            return elseBlock_.render(context);
        }
        int index0 = 0;
        if (reversed_) {
            for (int i = end; i >= start; --i, ++index0) {
                Data::Hash forloop;
                forloop["first"] = i == end;
                forloop["last"] = i == start;
                forloop["length"] = len;
                const int rindex0 = len - index0 - 1;
                forloop["index0"] = index0;
                forloop["index"] = index0 + 1;
                forloop["rindex0"] = rindex0;
                forloop["rindex"] = rindex0 + 1;
                data.insert("forloop", forloop);

                data.insert(varName, collection.at(static_cast<size_t>(i)));
                
                output += body_.render(context);
            }
        } else {
            for (int i = start; i <= end; ++i, ++index0) {
                Data::Hash forloop;
                forloop["first"] = i == start;
                forloop["last"] = i == end;
                forloop["length"] = len;
                const int rindex0 = len - index0 - 1;
                forloop["index0"] = index0;
                forloop["index"] = index0 + 1;
                forloop["rindex0"] = rindex0;
                forloop["rindex"] = rindex0 + 1;
                data.insert("forloop", forloop);

                data.insert(varName, collection.at(static_cast<size_t>(i)));

                output += body_.render(context);
            }
        }
    }
    return output;
}

void Liquid::ForTag::handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer)
{
    (void)tagName;
    (void)markup;
    (void)tokenizer;
}


#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::For") {
    
    SECTION("For") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} yo {%endfor%}",
            " yo  yo  yo  yo ",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3, 4}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}yo{%endfor%}",
            "yoyo",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} yo {%endfor%}",
            " yo ",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{%endfor%}",
            "",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2}}})
        );
    }

    SECTION("ForRange") {
        CHECK_TEMPLATE_RESULT(
            "{%for i in (1..2) %}{% assign a = 'variable'%}{% endfor %}{{a}}",
            "variable"
        );
        CHECK_TEMPLATE_RESULT(
            "{%for item in (1..3) %} {{item}} {%endfor%}",
            " 1  2  3 "
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in (1..foobar) %} {{item}} {%endfor%}",
            " 1  2  3 ",
            (Liquid::Data::Hash{{"foobar", 3}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in (1..foobar.value) %} {{item}} {%endfor%}",
            " 1  2  3 ",
            (Liquid::Data::Hash{{"foobar", Liquid::Data::Hash{{"value", 3}}}})
        );
    }
    
    SECTION("ForReversed") {
        CHECK_TEMPLATE_RESULT(
            "{%for item in (1..3) reversed %} {{item}} {%endfor%}",
            " 3  2  1 "
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed %}{{item}}{%endfor%}",
            "321",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
    }
    
    SECTION("ForVariable") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{item}} {%endfor%}",
            " 1  2  3 ",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "123",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{% for item in array %}{{item}}{% endfor %}",
            "123",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "abcd",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{"a", "b", "c", "d"}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "a b c",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{"a", " ", "b", " ", "c"}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{{item}}{%endfor%}",
            "abc",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{"a", "", "b", "", "c"}}})
        );
    }
    
    SECTION("ForloopObject") {
        Liquid::Data::Hash hash;
        hash["array"] = Liquid::Data::Array{1, 2, 3};
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{forloop.index}}/{{forloop.length}} {%endfor%}",
            " 1/3  2/3  3/3 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{forloop.index}} {%endfor%}",
            " 1  2  3 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{forloop.index0}} {%endfor%}",
            " 0  1  2 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{forloop.rindex0}} {%endfor%}",
            " 2  1  0 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{forloop.rindex}} {%endfor%}",
            " 3  2  1 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{forloop.first}} {%endfor%}",
            " true  false  false ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%} {{forloop.last}} {%endfor%}",
            " false  false  true ",
            hash
        );
    }
    
    SECTION("ForloopObjectReversed") {
        Liquid::Data::Hash hash;
        hash["array"] = Liquid::Data::Array{1, 2, 3};
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed%} {{forloop.index}}/{{forloop.length}} {%endfor%}",
            " 1/3  2/3  3/3 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed%} {{forloop.index}} {%endfor%}",
            " 1  2  3 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed%} {{forloop.index0}} {%endfor%}",
            " 0  1  2 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed%} {{forloop.rindex0}} {%endfor%}",
            " 2  1  0 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed%} {{forloop.rindex}} {%endfor%}",
            " 3  2  1 ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed%} {{forloop.first}} {%endfor%}",
            " true  false  false ",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array reversed%} {{forloop.last}} {%endfor%}",
            " false  false  true ",
            hash
        );
    }
    
    // TODO: test_for_and_if

    SECTION("ForElse") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}+{%else%}-{%endfor%}",
            "+++",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 2, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}+{%else%}-{%endfor%}",
            "-",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}+{%else%}-{%endfor%}",
            "-",
            (Liquid::Data::Hash{{"array", nullptr}})
        );
    }
    
    SECTION("ForLimiting") {
        Liquid::Data::Hash hash;
        hash["array"] = Liquid::Data::Array{1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for i in array limit:2 %}{{ i }}{%endfor%}",
            "12",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for i in array limit:4 %}{{ i }}{%endfor%}",
            "1234",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for i in array limit:4 offset:2 %}{{ i }}{%endfor%}",
            "3456",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for i in array limit: 4 offset: 2 %}{{ i }}{%endfor%}",
            "3456",
            hash
        );
        hash["limit"] = 2;
        hash["offset"] = 2;
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for i in array limit: limit offset: offset %}{{ i }}{%endfor%}",
            "34",
            hash
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for i in array offset:7 %}{{ i }}{%endfor%}",
            "890",
            hash
        );
    }
    
    SECTION("ForNested") {
        Liquid::Data::Hash hash;
        hash["array"] = Liquid::Data::Array{Liquid::Data::Array{1, 2}, Liquid::Data::Array{3, 4}, Liquid::Data::Array{5, 6}};
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{%for i in item%}{{ i }}{%endfor%}{%endfor%}",
            "123456",
            hash
        );
    }
    
    // TODO: test_pause_resume*
    
    
}

#endif
