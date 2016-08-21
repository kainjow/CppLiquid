#include "standardfilters.hpp"
#include "stringutils.hpp"
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

Data downcase(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("downcase doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().toLower();
}

Data upcase(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("upcase doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().toUpper();
}

Data capitalize(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("capitalize doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString str = input.toString();
    return str.left(1).toUpper() + str.mid(1);
}

Data strip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("strip doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().trimmed();
}

Data rstrip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("rstrip doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString str = input.toString();
    return rtrim(&str).toString();
}

Data lstrip(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("lstrip doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString str = input.toString();
    return ltrim(&str).toString();
}

Data strip_newlines(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("strip_newlines doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    QString str = input.toString();
    str.replace("\n", "");
    str.replace("\r", "");
    return str;
}

Data newline_to_br(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("newline_to_br doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().replace("\n", "<br />\n");
}

void registerFilters(Template& tmpl)
{
    tmpl.registerFilter("append", append);
    tmpl.registerFilter("prepend", prepend);
    tmpl.registerFilter("downcase", downcase);
    tmpl.registerFilter("upcase", upcase);
    tmpl.registerFilter("capitalize", capitalize);
    tmpl.registerFilter("strip", strip);
    tmpl.registerFilter("rstrip", rstrip);
    tmpl.registerFilter("lstrip", lstrip);
    tmpl.registerFilter("strip_newlines", strip_newlines);
    tmpl.registerFilter("newline_to_br", newline_to_br);
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

    SECTION("Downcase") {
        Liquid::Template t;
        t.parse("{{ \"HELLO\" | downcase }}");
        CHECK(t.render().toStdString() == "hello");
    }

    SECTION("Upcase") {
        Liquid::Template t;
        t.parse("{{ \"hello\" | upcase }}");
        CHECK(t.render().toStdString() == "HELLO");
    }

    SECTION("Capitalize") {
        Liquid::Template t;
        t.parse("{{ \"hello world\" | capitalize }}");
        CHECK(t.render().toStdString() == "Hello world");
    }

    SECTION("Strip") {
        Liquid::Template t;
        t.parse("{{ \" \r\n\thello\t\n\r \" | strip }}");
        CHECK(t.render().toStdString() == "hello");
    }

    SECTION("Rstrip") {
        Liquid::Template t;
        t.parse("{{ \" \r\n\thello\t\n\r \" | rstrip }}");
        CHECK(t.render().toStdString() == " \r\n\thello");
    }

    SECTION("Lstrip") {
        Liquid::Template t;
        t.parse("{{ \" \r\n\thello\t\n\r \" | lstrip }}");
        CHECK(t.render().toStdString() == "hello\t\n\r ");
    }

    SECTION("StripNewLines") {
        Liquid::Template t;
        t.parse("{{ \"\r\nhe\nll\ro\r\" | strip_newlines }}");
        CHECK(t.render().toStdString() == "hello");
    }

    SECTION("NewlineToBr") {
        Liquid::Template t;
        t.parse("{{ \"hello\nand\ngoodbye\n\" | newline_to_br }}");
        CHECK(t.render().toStdString() == "hello<br />\nand<br />\ngoodbye<br />\n");
    }

    
}

#endif
