#include "template.hpp"



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Comment") {
    
    SECTION("Comments") {
        Liquid::Template t;
        CHECK(t.parse("{% comment %}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("hello {% comment %}{% endcomment %} world").render().toStdString() == "hello  world");
        CHECK(t.parse("one {% comment %} two {% endcomment %} three").render().toStdString() == "one  three");
        CHECK(t.parse("{% comment %}In Mustache, {{ this }} will be HTML-escaped, but {{{ that }}} will not.{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{% comment %} test {% endcomment %}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{{ test }}{% endcomment %}").render().toStdString() == "");
        
        CHECK(t.parse("the comment block should be removed {%comment%} be gone.. {%endcomment%} .. right?").render().toStdString() == "the comment block should be removed  .. right?");
        CHECK(t.parse("{%comment%}{%endcomment%}").render().toStdString() == "");
        CHECK(t.parse("{%comment%}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{%endcomment%}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{%comment%}comment{%endcomment%}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}comment{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} 1 {% comment %} 2 {% endcomment %} 3 {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{%comment%}{%blabla%}{%endcomment%}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{% blabla %}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{%comment%}{% endif %}{%endcomment%}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{% endwhatever %}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{% raw %} {{%%%%}}  }} { {% endcomment %} {% comment {% endraw %} {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("foo{%comment%}comment{%endcomment%}bar").render().toStdString() == "foobar");
        CHECK(t.parse("foo{% comment %}comment{% endcomment %}bar").render().toStdString() == "foobar");
        CHECK(t.parse("foo{%comment%} comment {%endcomment%}bar").render().toStdString() == "foobar");
        CHECK(t.parse("foo{% comment %} comment {% endcomment %}bar").render().toStdString() == "foobar");
        CHECK(t.parse("foo {%comment%} {%endcomment%} bar").render().toStdString() == "foo  bar");
        CHECK(t.parse("foo {%comment%}comment{%endcomment%} bar").render().toStdString() == "foo  bar");
        CHECK(t.parse("foo {%comment%} comment {%endcomment%} bar").render().toStdString() == "foo  bar");
        CHECK(t.parse("foo{%comment%}\n\n\n\n{%endcomment%}bar").render().toStdString() == "foobar");
    }
}

#endif
