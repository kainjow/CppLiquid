#include "break.hpp"
#include "context.hpp"

Liquid::String Liquid::BreakTag::render(Context& ctx)
{
    ctx.push_interrupt(Context::Interrupt::Break);
    return "";
}
