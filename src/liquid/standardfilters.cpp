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

void registerFilters(Template& tmpl)
{
    tmpl.registerFilter("append", append);
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

}

#endif
