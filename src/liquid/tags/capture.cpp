#include "capture.hpp"
#include "parser.hpp"
#include "context.hpp"
#include "template.hpp"

Liquid::CaptureTag::CaptureTag(const QStringRef& tagName, Parser& parser)
    : BlockTag(tagName, parser)
{
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::EndOfString);
}

QString Liquid::CaptureTag::render(Context& context)
{
    const QString output = BlockTag::render(context);
    context.data().insert(to_.toString(), output);
    return "";
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Capture") {
    
    SECTION("Capture") {
        Liquid::Template t;
        CHECK(t.parse("{% capture my_variable %}I am being captured.{% endcapture %}{{ my_variable }}").render().toStdString() == "I am being captured.");

        Liquid::Data::Hash hash;
        hash["var"] = "content";
        Liquid::Data data(hash);
        CHECK(t.parse("{{ var2 }}{% capture var2 %}{{ var }} foo {% endcapture %}{{ var2 }}{{ var2 }}").render(data).toStdString() == "content foo content foo ");
    }
}

#endif
