#include "node.hpp"
#include "context.hpp"
#include <QDebug>

Liquid::TextNode::TextNode(const QStringRef& text)
    : text_(text)
{
}
    
QString Liquid::TextNode::render(Context&)
{
    return text_.toString();
}
    
Liquid::ObjectNode::ObjectNode(const Variable& var)
    : var_(var)
{
}
    
QString Liquid::ObjectNode::render(Context& context)
{
    return var_.evaluate(context).toString();
}

Liquid::AssignTag::AssignTag(Parser& parser)
{
    to_ = parser.consume(Token::Type::Id);
    (void)parser.consume(Token::Type::Equal);
    from_ = Variable(parser);
}
    
QString Liquid::AssignTag::render(Context& ctx)
{
    Data& data = ctx.data();
    data.insert(to_.toString(), from_.evaluate(ctx));
    return "";
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Node") {
    

}

#endif
