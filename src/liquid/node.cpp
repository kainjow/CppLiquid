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

QString Liquid::TagNode::render(Context&)
{
    return "";
}




#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Node") {
    

}

#endif
