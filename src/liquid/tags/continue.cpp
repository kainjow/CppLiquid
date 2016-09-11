#include "continue.hpp"
#include "context.hpp"

QString Liquid::ContinueTag::render(Context& ctx)
{
    ctx.push_interrupt(Context::Interrupt::Continue);
    return "";
}
