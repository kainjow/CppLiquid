#include "catch.hpp"
#include "tokenizer.hpp"

namespace {

    QStringList tokenize(const QString& source) {
        Liquid::Tokenizer t(source);
        QStringList tokens;
        QString token;
        while (t.shift(token)) {
            tokens << token;
        }
        return tokens;
    }

}

TEST_CASE("Tokenizer") {
    SECTION("strings") {
        CHECK(QStringList{""} == tokenize(""));
        CHECK(QStringList{" "} == tokenize(" "));
        CHECK(QStringList{"hello world"} == tokenize("hello world"));
    }
    
    SECTION("variables") {
        CHECK(QStringList{"{{funk}}"} == tokenize("{{funk}}"));
        QStringList expected = QStringList{" ", "{{funk}}", " "};
        CHECK(expected == tokenize(" {{funk}} "));
        expected = QStringList{" ", "{{funk}}", " ", "{{so}}", " ", "{{brother}}", " "};
        CHECK(expected == tokenize(" {{funk}} {{so}} {{brother}} "));
        expected = QStringList{" ", "{{  funk  }}", " "};
        CHECK(expected == tokenize(" {{  funk  }} "));
    }
}
