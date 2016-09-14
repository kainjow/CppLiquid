#include "ifchanged.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::IfchangedTag::IfchangedTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
    : BlockTag(context, tagName, markup)
{
}

QString Liquid::IfchangedTag::render(Context& context)
{
    const QString output = BlockTag::render(context);
    Data::Hash& registers = context.registers();
    const Data& current = registers["ifchanged"];
    if (current.toString() != output) {
        registers["ifchanged"] = output;
        return output;
    }
    return "";
}



#ifdef TESTS

#include "tests.hpp"

TEST_CASE("Liquid::Ifchanged") {
    
    SECTION("Ifchanged") {
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{%ifchanged%}{{item}}{% endifchanged %}{%endfor%}",
            "123",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 1, 2, 2, 3, 3}}})
        );
        CHECK_TEMPLATE_DATA_RESULT(
            "{%for item in array%}{%ifchanged%}{{item}}{% endifchanged %}{%endfor%}",
            "1",
            (Liquid::Data::Hash{{"array", Liquid::Data::Array{1, 1, 1, 1}}})
        );
    }
}

#endif
