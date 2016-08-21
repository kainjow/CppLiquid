#include "standardfilters.hpp"
#include <QDebug>

namespace Liquid { namespace StandardFilters {

Data append(const Data& input, const std::vector<Data>& args)
{
    Data value = input;
    for (const auto& arg : args) {
        value = value.toString() + arg.toString();
    }
    return value;
}

Data prepend(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("prepend only takes one argument, but was passed %1.").arg(args.size()).toStdString();
    }
    return args[0].toString() + input.toString();
}

void registerFilters(Template& tmpl)
{
    tmpl.registerFilter("append", append);
    tmpl.registerFilter("prepend", prepend);
}

} } // namespace



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::StandardFilters") {
    
    SECTION("AppendToString") {
        Liquid::Template t;
        t.parse("{{ \"hello \" | append: \"world\" }}");
        CHECK(t.render().toStdString() == "hello world");
    }

    SECTION("AppendToInt") {
        Liquid::Template t;
        t.parse("{{ 32 | append: \"world\" }}");
        CHECK(t.render().toStdString() == "32world");
    }

    SECTION("AppendToFloat") {
        Liquid::Template t;
        t.parse("{{ 32.94 | append: \"world\" }}");
        CHECK(t.render().toStdString() == "32.94world");
    }

    SECTION("Prepend") {
        Liquid::Template t;
        t.parse("{{ \" world\" | prepend: \"hello\" }}");
        CHECK(t.render().toStdString() == "hello world");
    }
}

#endif
