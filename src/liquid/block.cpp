#include "block.hpp"

Liquid::BlockTag::BlockTag(const Context& context, const StringRef& tagName, const StringRef& markup)
    : TagNode(context, tagName, markup)
    , tagName_(tagName)
{
}

void Liquid::BlockTag::parse(const Context& context, Tokenizer& tokenizer)
{
    while (parseBody(context, &body_, tokenizer)) {
    }
}

void Liquid::BlockTag::handleUnknownTag(const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer)
{
    BlockBody::defaultUnknownTagHandler(tagName, markup, tokenizer);
}

bool Liquid::BlockTag::parseBody(const Context& context, BlockBody* body, Tokenizer& tokenizer)
{
    if (!body) {
        throw std::string("body cannot be null");
    }
    const String endTagName = String("end") + tagName_.toString();
    bool ret = true;
    body->parse(context, tokenizer, [endTagName, &ret, this](const StringRef& tagName, const StringRef& markup, Tokenizer& tokenizer) {
        if (tagName == endTagName) {
            ret = false;
            return;
        }
        if (tagName.isEmpty()) {
            throw String("%1 tag was never closed").arg(tagName_.toString()).toStdString();
        }
        handleUnknownTag(tagName, markup, tokenizer);
    });
    return ret;
}

Liquid::String Liquid::BlockTag::render(Context& context)
{
    return body_.render(context);
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Block") {
    

}

#endif
