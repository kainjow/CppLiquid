#include "block.hpp"

Liquid::BlockTag::BlockTag(const QStringRef& tagName, const QStringRef& markup)
    : TagNode(tagName, markup)
    , tagName_(tagName)
{
}

void Liquid::BlockTag::parse(Tokenizer& tokenizer)
{
    while (parseBody(body_, tokenizer)) {
    }
}

void Liquid::BlockTag::handleUnknownTag(const QStringRef& tagName, Tokenizer& tokenizer)
{
    BlockBody::defaultUnknownTagHandler(tagName, tokenizer);
}

bool Liquid::BlockTag::parseBody(BlockBody& body, Tokenizer& tokenizer)
{
    const QString endTagName = "end" + tagName_.toString();
    bool ret = true;
    body.parse(tokenizer, [endTagName, &ret, this](const QStringRef& tagName, Tokenizer& tokenizer) {
        if (tagName == endTagName) {
            ret = false;
            return;
        }
        if (tagName.isNull()) {
            throw QString("%1 tag was never closed").arg(tagName_.toString()).toStdString();
        }
        handleUnknownTag(tagName, tokenizer);
    });
    return ret;
}

QString Liquid::BlockTag::render(Context& context)
{
    return body_.render(context);
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Block") {
    

}

#endif
