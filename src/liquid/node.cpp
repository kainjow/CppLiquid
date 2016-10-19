#include "node.hpp"
#include "context.hpp"

Liquid::TextNode::TextNode(const Context& context, const StringRef& text)
    : Node(context)
    , text_(text)
{
}
    
Liquid::String Liquid::TextNode::render(Context&)
{
    return text_.toString();
}
    
Liquid::ObjectNode::ObjectNode(const Context& context, const Variable& var)
    : Node(context)
    , var_(var)
{
}
    
Liquid::String Liquid::ObjectNode::render(Context& context)
{
    return var_.evaluate(context).toString();
}

Liquid::String Liquid::TagNode::render(Context&)
{
    return "";
}




#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Node") {
    

}

#endif
