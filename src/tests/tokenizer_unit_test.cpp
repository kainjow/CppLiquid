#include "catch.hpp"
#include "tokenizer.hpp"

namespace {

    std::vector<QString> tokenize(const QString& source) {
        Liquid::Tokenizer t(source);
        std::vector<QString> tokens;
        QString token;
        while (t.shift(token)) {
            tokens.emplace_back(token);
        }
        return tokens;
    }

}

TEST_CASE("Tokenizer") {
    std::vector<QString> expected;

    SECTION("strings") {
        CHECK(std::vector<QString>{""} == tokenize(""));
        CHECK(std::vector<QString>{" "} == tokenize(" "));
        CHECK(std::vector<QString>{"hello world"} == tokenize("hello world"));
    }
    
    SECTION("variables") {
        expected = {"{{funk}}"};
        CHECK(expected == tokenize("{{funk}}"));

        expected = {" ", "{{funk}}", " "};
        CHECK(expected == tokenize(" {{funk}} "));

        expected = {" ", "{{funk}}", " ", "{{so}}", " ", "{{brother}}", " "};
        CHECK(expected == tokenize(" {{funk}} {{so}} {{brother}} "));

        expected = {" ", "{{  funk  }}", " "};
        CHECK(expected == tokenize(" {{  funk  }} "));
    }
    
    SECTION("blocks") {
        expected = {"{%comment%}"};
        CHECK(expected == tokenize("{%comment%}"));

        expected = {" ", "{%comment%}", " "};
        CHECK(expected == tokenize(" {%comment%} "));

        expected = {" ", "{%comment%}", " ", "{%endcomment%}", " "};
        CHECK(expected == tokenize(" {%comment%} {%endcomment%} "));

        expected = {"  ", "{% comment %}", " ", "{% endcomment %}", " "};
        CHECK(expected == tokenize("  {% comment %} {% endcomment %} "));
    }
}
