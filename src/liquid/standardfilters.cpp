#include "standardfilters.hpp"
#include "stringutils.hpp"
#include <QDebug>
#include <QUrl>

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
    return input.toString().replace("\n", "").replace("\r", "");
}

Data newline_to_br(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("newline_to_br doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().replace("\n", "<br />\n");
}

Data escape(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("escape doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return input.toString().toHtmlEscaped().replace("'", "&#39;");
}

Data url_encode(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("url_encode doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    return QString::fromUtf8(QUrl::toPercentEncoding(input.toString().toUtf8()));
}

Data strip_html(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("url_encode doesn't take any arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const QString inputStr = input.toString();
    const int inputStrSize = inputStr.size();
    QString output;
    bool html = false;
    for (int i = 0; i < inputStrSize; ++i) {
        const QChar ch = inputStr[i];
        if (ch == '<') {
            html = true;
        } else if (ch == '>') {
            html = false;
        } else if (!html) {
            output += ch;
        }
    }
    return output;
}

Data truncate(const Data& input, const std::vector<Data>& args)
{
    if (args.size() < 1 || args.size() > 2) {
        throw QString("truncate takes 1 or 2 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    const int length = args[0].toInt();
    const QString truncateStr = args.size() == 2 ? args[1].toString() : "...";
    int len = length - truncateStr.size();
    if (len < 0) {
        len = 0;
    }
    return input.toString().left(len) + truncateStr;
}

Data plus(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("plus takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() + arg.toInt();
    }
    return input.toFloat() + arg.toFloat();
}

Data minus(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("minus takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() - arg.toInt();
    }
    return input.toFloat() - arg.toFloat();
}

Data times(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("times takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() * arg.toInt();
    }
    return input.toFloat() * arg.toFloat();
}

Data divided_by(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 1) {
        throw QString("divided_by takes 1 argument, but was passed %1.").arg(args.size()).toStdString();
    }
    const Data& arg = args[0];
    if (input.isNumberInt() && arg.isNumberInt()) {
        return input.toInt() / arg.toInt();
    }
    return input.toFloat() / arg.toFloat();
}

Data abs(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("abs takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return ::abs(input.toInt());
    }
    return ::fabs(input.toFloat());
}

Data ceil(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("ceil takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    return ::ceil(input.toFloat());
}

Data floor(const Data& input, const std::vector<Data>& args)
{
    if (args.size() != 0) {
        throw QString("ceil takes 0 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    return ::floor(input.toFloat());
}

Data round(const Data& input, const std::vector<Data>& args)
{
    if (args.size() > 1) {
        throw QString("round takes 0 or 1 arguments, but was passed %1.").arg(args.size()).toStdString();
    }
    if (input.isNumberInt()) {
        return input.toInt();
    }
    if (args.size() == 1) {
        const Data& arg = args[0];
        return doubleToString(input.toFloat(), arg.toInt());
    }
    return ::round(input.toFloat());
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
    tmpl.registerFilter("escape", escape);
    tmpl.registerFilter("url_encode", url_encode);
    tmpl.registerFilter("strip_html", strip_html);
    tmpl.registerFilter("truncate", truncate);
    tmpl.registerFilter("plus", plus);
    tmpl.registerFilter("minus", minus);
    tmpl.registerFilter("times", times);
    tmpl.registerFilter("divided_by", divided_by);
    tmpl.registerFilter("abs", abs);
    tmpl.registerFilter("ceil", ceil);
    tmpl.registerFilter("floor", floor);
    tmpl.registerFilter("round", round);
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

    SECTION("Escape") {
        Liquid::Template t;
        Liquid::Data::Hash hash;
        hash["what"] = "' \" & < > ' \" & < >";
        t.parse("{{ what | escape }}");
        CHECK(t.render(hash).toStdString() == "&#39; &quot; &amp; &lt; &gt; &#39; &quot; &amp; &lt; &gt;");
    }

    SECTION("UrlEncode") {
        Liquid::Template t;
        t.parse("{{ \"hello @world\" | url_encode }}");
        CHECK(t.render().toStdString() == "hello%20%40world");
    }

    SECTION("StripHtml") {
        Liquid::Template t;
        t.parse("{{ \"<p>hello <b>w<span>or</span>ld</b></p>\" | strip_html }}");
        CHECK(t.render().toStdString() == "hello world");
    }

    SECTION("Truncate") {
        Liquid::Template t;
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 20 }}");
        CHECK(t.render().toStdString() == "Ground control to...");
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 25, \", and so on\" }}");
        CHECK(t.render().toStdString() == "Ground control, and so on");
        t.parse("{{ \"Ground control to Major Tom.\" | truncate: 20, \"\" }}");
        CHECK(t.render().toStdString() == "Ground control to Ma");
    }

    SECTION("Plus") {
        Liquid::Template t;
        t.parse("{{ 4 | plus: 2 }}");
        CHECK(t.render().toStdString() == "6");
        t.parse("{{ 16 | plus: 4 }}");
        CHECK(t.render().toStdString() == "20");
        t.parse("{{ 183.357 | plus: 12 }}");
        CHECK(t.render().toStdString() == "195.357");
    }

    SECTION("Minus") {
        Liquid::Template t;
        t.parse("{{ 4 | minus: 2 }}");
        CHECK(t.render().toStdString() == "2");
        t.parse("{{ 16 | minus: 4 }}");
        CHECK(t.render().toStdString() == "12");
        t.parse("{{ 183.357 | minus: 12 }}");
        CHECK(t.render().toStdString() == "171.357");
    }

    SECTION("Times") {
        Liquid::Template t;
        t.parse("{{ 4 | times: 2 }}");
        CHECK(t.render().toStdString() == "8");
        t.parse("{{ 16 | times: 4 }}");
        CHECK(t.render().toStdString() == "64");
        t.parse("{{ 183.357 | times: 12 }}");
        CHECK(t.render().toStdString() == "2200.284");
    }

    SECTION("DividedBy") {
        Liquid::Template t;
        t.parse("{{ 16 | divided_by: 4 }}");
        CHECK(t.render().toStdString() == "4");
        t.parse("{{ 5 | divided_by: 3 }}");
        CHECK(t.render().toStdString() == "1");
        t.parse("{{ 20 | divided_by: 7 }}");
        CHECK(t.render().toStdString() == "2");
        t.parse("{{ 20 | divided_by: 7.0 }}");
        CHECK(t.render().toStdString() == "2.857143");
    }

    SECTION("Abs") {
        Liquid::Template t;
        CHECK(t.parse("{{ -17 | abs }}").render().toStdString() == "17");
        CHECK(t.parse("{{ 4 | abs }}").render().toStdString() == "4");
        CHECK(t.parse("{{ -19.86 | abs }}").render().toStdString() == "19.86");
    }

    SECTION("Ceil") {
        Liquid::Template t;
        CHECK(t.parse("{{ 1.2 | ceil }}").render().toStdString() == "2");
        CHECK(t.parse("{{ 2.0 | ceil }}").render().toStdString() == "2");
        CHECK(t.parse("{{ 183.357 | ceil }}").render().toStdString() == "184");
    }

    SECTION("Floor") {
        Liquid::Template t;
        CHECK(t.parse("{{ 1.2 | floor }}").render().toStdString() == "1");
        CHECK(t.parse("{{ 2.0 | floor }}").render().toStdString() == "2");
        CHECK(t.parse("{{ 183.357 | floor }}").render().toStdString() == "183");
    }

    SECTION("Round") {
        Liquid::Template t;
        CHECK(t.parse("{{ 1.2 | round }}").render().toStdString() == "1");
        CHECK(t.parse("{{ 2.7 | round }}").render().toStdString() == "3");
        CHECK(t.parse("{{ 183.357 | round: 1}}").render().toStdString() == "183.4");
        CHECK(t.parse("{{ 183.357 | round: 2}}").render().toStdString() == "183.36");
    }

}

#endif
