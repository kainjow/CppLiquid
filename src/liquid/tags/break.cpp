#include "break.hpp"
#include "context.hpp"

QString Liquid::BreakTag::render(Context& ctx)
{
    ctx.push_interrupt(Context::Interrupt::Break);
    return "";
}
