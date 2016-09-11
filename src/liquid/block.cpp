#include "block.hpp"

Liquid::BlockTag::BlockTag(const Context& context, const QStringRef& tagName, const QStringRef& markup)
    : TagNode(context, tagName, markup)
    , tagName_(tagName)
{
}

void Liquid::BlockTag::parse(const Context& context, Tokenizer& tokenizer)
{
    while (parseBody(context, &body_, tokenizer)) {
    }
}

void Liquid::BlockTag::handleUnknownTag(const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer)
{
    BlockBody::defaultUnknownTagHandler(tagName, markup, tokenizer);
}

bool Liquid::BlockTag::parseBody(const Context& context, BlockBody* body, Tokenizer& tokenizer)
{
    if (!body) {
        throw std::string("body cannot be null");
    }
    const QString endTagName = "end" + tagName_.toString();
    bool ret = true;
    body->parse(context, tokenizer, [endTagName, &ret, this](const QStringRef& tagName, const QStringRef& markup, Tokenizer& tokenizer) {
        if (tagName == endTagName) {
            ret = false;
            return;
        }
        if (tagName.isNull()) {
            throw QString("%1 tag was never closed").arg(tagName_.toString()).toStdString();
        }
        handleUnknownTag(tagName, markup, tokenizer);
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
