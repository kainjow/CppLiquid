#include "continue.hpp"
#include "context.hpp"

Liquid::String Liquid::ContinueTag::render(Context& ctx)
{
    ctx.push_interrupt(Context::Interrupt::Continue);
    return "";
}
