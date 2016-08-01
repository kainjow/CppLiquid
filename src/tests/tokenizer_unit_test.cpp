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
    SECTION("strings") {
        CHECK(std::vector<QString>{""} == tokenize(""));
        CHECK(std::vector<QString>{" "} == tokenize(" "));
        CHECK(std::vector<QString>{"hello world"} == tokenize("hello world"));
    }
    
    SECTION("variables") {
        CHECK(std::vector<QString>{"{{funk}}"} == tokenize("{{funk}}"));
        
        std::vector<QString> expected;
        
        expected = std::vector<QString>{" ", "{{funk}}", " "};
        CHECK(expected == tokenize(" {{funk}} "));
        
        expected = std::vector<QString>{" ", "{{funk}}", " ", "{{so}}", " ", "{{brother}}", " "};
        CHECK(expected == tokenize(" {{funk}} {{so}} {{brother}} "));

        expected = std::vector<QString>{" ", "{{  funk  }}", " "};
        CHECK(expected == tokenize(" {{  funk  }} "));
    }
}
