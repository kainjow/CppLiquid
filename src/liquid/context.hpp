#ifndef LIQUID_CONTEXT_HPP
#define LIQUID_CONTEXT_HPP

#include "filter.hpp"

namespace Liquid {
    
    class Context {
    public:
        Context(Data& data, const FilterList& filters)
            : data_(data)
            , filters_(filters)
        {
        }
        
        const Data& data() const {
            return data_;
        }
        
        Data& data() {
            return data_;
        }
        
        const FilterList& filters() const {
            return filters_;
        }
        
        Data::Hash& environments() {
            return environments_;
        }
        
        Data::Hash& registers() {
            return registers_;
        }
        
    private:
        Data& data_;
        Data::Hash environments_;
        Data::Hash registers_;
        const FilterList& filters_;
    };

}

#endif
