#include "drop.hpp"
#include "data.hpp"

const Liquid::Data& Liquid::Drop::operator[](const QString& key) const
{
    const auto it = cache_.find(key);
    if (it == cache_.end()) {
        const Data val = load(key);
        const auto inserted = cache_.insert(std::make_pair(key, val));
        return inserted.first->second;
    }
    return it->second;
}

bool Liquid::Drop::operator==(const Drop& other) const
{
    return cache_ == other.cache_;
}

Liquid::Data Liquid::Drop::load(const QString&) const
{
    return kNilData;
}

Liquid::DropHandler::DropHandler(const Loader& loader)
    : loader_(loader)
{
}

Liquid::Data Liquid::DropHandler::load(const QString& key) const
{
    return loader_(key);
}
