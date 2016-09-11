#include "node.hpp"
#include "context.hpp"
#include <QDebug>

Liquid::TextNode::TextNode(const Context& context, const QStringRef& text)
    : Node(context)
    , text_(text)
{
}
    
QString Liquid::TextNode::render(Context&)
{
    return text_.toString();
}
    
Liquid::ObjectNode::ObjectNode(const Context& context, const Variable& var)
    : Node(context)
    , var_(var)
{
}
    
QString Liquid::ObjectNode::render(Context& context)
{
    return var_.evaluate(context).toString();
}

QString Liquid::TagNode::render(Context&)
{
    return "";
}




#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Node") {
    

}

#endif
