#ifndef LIQUID_FILTER_HPP
#define LIQUID_FILTER_HPP

#include "data.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace Liquid {
    
    using FilterHandler = std::function<Data(const Data& input, const std::vector<Data>&)>;
    
    using FilterList = std::unordered_map<std::string, FilterHandler>;

}

#endif
