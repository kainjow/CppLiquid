#include "string.hpp"

Liquid::StringRef Liquid::String::midRef(size_type pos, size_type num) const
{
    if (pos > size()) {
        return {};
    }
    const size_type len = num == static_cast<size_type>(-1) || size() < num ? size() - pos : num;
    return StringRef(this, pos, len);
}
