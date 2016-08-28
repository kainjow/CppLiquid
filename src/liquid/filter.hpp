#ifndef LIQUID_FILTER_HPP
#define LIQUID_FILTER_HPP

#include "data.hpp"
#include "expression.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace Liquid {
    
    using FilterHandler = std::function<Data(const Data& input, const std::vector<Data>&)>;
    
    using FilterList = std::unordered_map<std::string, FilterHandler>;

    class Filter {
    public:
        Filter(const QStringRef& name, const std::vector<Expression>& args)
            : name_(name)
            , args_(args)
        {
        }
        
        const QStringRef& name() const {
            return name_;
        }
        
        const std::vector<Expression>& args() const {
            return args_;
        }
        
    private:
        const QStringRef name_;
        std::vector<Expression> args_;
    };
    
}

#endif
