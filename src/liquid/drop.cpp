#include "drop.hpp"
#include "data.hpp"

const Liquid::Data& Liquid::Drop::operator[](const QString& key) const
{
    const Data val = load(key);
    const auto it = storage_.find(key);
    if (it == storage_.end() || it->second != val) {
        return storage_.insert(std::make_pair(key, val)).first->second;
    }
    return it->second;
}

bool Liquid::Drop::operator==(const Drop& other) const
{
    return storage_ == other.storage_;
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
