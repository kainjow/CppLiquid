#include "template.hpp"
#include "standardfilters.hpp"
#include "tokenizer.hpp"
#include "context.hpp"
#include <QDebug>

Liquid::Template::Template()
{
    StandardFilters::registerFilters(*this);
}

Liquid::Template& Liquid::Template::parse(const QString& source)
{
    source_ = source;
    Tokenizer tokenizer(source_);
    root_ = BlockBody(tokenizer);
    return *this;
}

QString Liquid::Template::render()
{
    Data ctx(Data::Type::Hash);
    return render(ctx);
}

QString Liquid::Template::render(Data& data)
{
    Context ctx(data, filters_);
    return root_.render(ctx);
}

void Liquid::Template::registerFilter(const std::string& name, const FilterHandler& filter)
{
    filters_[name] = filter;
}


#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Template") {
    
    SECTION("NoParse") {
        Liquid::Template t;
        CHECK(t.render() == "");
    }

    SECTION("Empty") {
        Liquid::Template t;
        t.parse("");
        CHECK(t.render() == "");
    }

    SECTION("PlainText") {
        Liquid::Template t;
        t.parse("Hello World!");
        CHECK(t.render() == "Hello World!");
    }

    SECTION("SingleObject") {
        Liquid::Template t;
        t.parse("{{what}}");
        CHECK(t.render() == "");
    }
    
    SECTION("SingleObjectWithText") {
        Liquid::Template t;
        t.parse("Hello {{what}}!");
        CHECK(t.render() == "Hello !");
    }

    SECTION("UnclosedObject") {
        Liquid::Template t;
        CHECK_THROWS_AS(t.parse("{{what"), std::string);
    }
    
    SECTION("BasicObject") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["what"] = "World";
        Liquid::Data ctx(hash);
        t.parse("Hello {{what}}!");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "Hello World!");
    }
    
    SECTION("BasicMultipleObjects") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["fname"] = "Bill";
        hash["lname"] = "Gates";
        Liquid::Data ctx(hash);
        t.parse("Dear {{ fname }} {{ lname }},");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "Dear Bill Gates,");
    }
    
    SECTION("ObjectKey") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        Liquid::Data::Hash user;
        user["name"] = "Bob";
        hash["user"] = user;
        Liquid::Data ctx(hash);
        t.parse("Welcome {{user.name}}!");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "Welcome Bob!");
    }

    SECTION("ObjectBracketKeyStringLiteral") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        Liquid::Data::Hash user;
        user["name"] = "Bob";
        hash["user"] = user;
        Liquid::Data ctx(hash);
        t.parse("Welcome {{user[\"name\"]}}!");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "Welcome Bob!");
    }

    SECTION("ObjectBracketKeyObjectNil") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        Liquid::Data::Hash user;
        user["name"] = "Bob";
        hash["user"] = user;
        Liquid::Data ctx(hash);
        t.parse("Welcome {{user[name]}}!");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "Welcome !");
    }

    SECTION("ObjectBracketKeyObjectSet") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        Liquid::Data::Hash user;
        user["name"] = "Bob";
        hash["user"] = user;
        hash["name"] = "name";
        Liquid::Data ctx(hash);
        t.parse("Welcome {{user[name]}}!");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "Welcome Bob!");
    }

    SECTION("ObjectMultipleBracketKey") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        Liquid::Data::Hash states;
        Liquid::Data::Hash california;
        Liquid::Data::Hash sacramento;
        Liquid::Data::Array keys;
        keys.push_back("arizona");
        keys.push_back("california");
        sacramento["areacode"] = "916";
        california["capital"] = sacramento;
        states["california"] = california;
        hash["states"] = states;
        hash["keys"] = keys;
        Liquid::Data ctx(hash);
        t.parse("{{states[keys[1]][\"capital\"][\"areacode\"]}} {{keys[0]}} {{keys[9999]}}");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "916 arizona ");
    }
    
    SECTION("AppendFilter") {
        Liquid::Template t;
        Liquid::Data ctx;
        t.parse("{{ \"hello\" | append: \"world\" }}");
        CHECK(t.render(ctx).toStdString() == "helloworld");
    }

    SECTION("AppendFilterMultiple") {
        Liquid::Template t;
        Liquid::Data ctx;
        t.parse("{{ \"hello\" | append: \"world\", \"planet\", \"galaxy\", \"universe\" }}");
        CHECK(t.render(ctx).toStdString() == "helloworldplanetgalaxyuniverse");
    }
    
    SECTION("AppendFilterObject") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["what"] = "world";
        Liquid::Data ctx(hash);
        t.parse("{{ \"hello \" | append: what }}");
        Liquid::Data data(hash);
        CHECK(t.render(data).toStdString() == "hello world");
    }

    SECTION("Floats") {
        Liquid::Template t;
        CHECK(t.parse("{{ 32 }}").render().toStdString() == "32");
        CHECK(t.parse("{{ 32. }}").render().toStdString() == "32");
        CHECK(t.parse("{{ 32.0 }}").render().toStdString() == "32");
        CHECK(t.parse("{{ 32.1 }}").render().toStdString() == "32.1");
    }
    
    SECTION("TagRaw") {
        Liquid::Template t;
        CHECK(t.parse("{% raw %}In Mustache, {{ this }} will be HTML-escaped, but {{{ that }}} will not.{% endraw %}").render().toStdString() == "In Mustache, {{ this }} will be HTML-escaped, but {{{ that }}} will not.");
        CHECK(t.parse("{% raw %}{% comment %} test {% endcomment %}{% endraw %}").render().toStdString() == "{% comment %} test {% endcomment %}");
        CHECK(t.parse("{% raw %}{{ test }}{% endraw %}").render().toStdString() == "{{ test }}");
        CHECK(t.parse("{% raw %} Foobar {% invalid {% endraw %}").render().toStdString() == " Foobar {% invalid ");
        CHECK(t.parse("{% raw %} Foobar invalid %} {% endraw %}").render().toStdString() == " Foobar invalid %} ");
        CHECK(t.parse("{% raw %} Foobar {{ invalid {% endraw %}").render().toStdString() == " Foobar {{ invalid ");
        CHECK(t.parse("{% raw %} Foobar invalid }} {% endraw %}").render().toStdString() == " Foobar invalid }} ");
        CHECK(t.parse("{% raw %} Foobar {% invalid {% {% endraw {% endraw %}").render().toStdString() == " Foobar {% invalid {% {% endraw ");
        CHECK(t.parse("{% raw %} Foobar {% {% {% {% endraw %}").render().toStdString() == " Foobar {% {% {% ");
        CHECK(t.parse("{% raw %} test {% raw %} {% {% endraw %}endraw %}").render().toStdString() == " test {% raw %} {% endraw %}");
        CHECK(t.parse("{% raw %} Foobar {{ invalid {% endraw %}{{ 1 }}").render().toStdString() == " Foobar {{ invalid 1");
    }

    SECTION("TagComment") {
        Liquid::Template t;
        CHECK(t.parse("{% comment %}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("hello {% comment %}{% endcomment %} world").render().toStdString() == "hello  world");
        CHECK(t.parse("one {% comment %} two {% endcomment %} three").render().toStdString() == "one  three");
        CHECK(t.parse("{% comment %}In Mustache, {{ this }} will be HTML-escaped, but {{{ that }}} will not.{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{% comment %} test {% endcomment %}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %}{{ test }}{% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} Foobar {% invalid {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} Foobar invalid %} {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} Foobar {{ invalid {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} Foobar invalid }} {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} Foobar {% invalid {% {% endraw {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} Foobar {% {% {% {% endcomment %}").render().toStdString() == "");
        CHECK(t.parse("{% comment %} test {% raw %} {% {% endcomment %}endraw %}").render().toStdString() == "endraw %}");
        CHECK(t.parse("{% comment %} Foobar {{ invalid {% endcomment %}{{ 1 }}").render().toStdString() == "1");
    }
    
    SECTION("TagAssign") {
        Liquid::Template t;
        CHECK(t.parse("{% assign name = 'Steve' %}{{ name }}").render().toStdString() == "Steve");
    }
}

#endif
