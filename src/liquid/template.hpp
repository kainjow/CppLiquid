#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include "data.hpp"
#include "filter.hpp"
#include "blockbody.hpp"

namespace Liquid {

    class Template {
    public:
        Template();
        
        Template& parse(const QString& source);
        
        QString render();
        QString render(const Data& data);
        
        void registerFilter(const std::string& name, const FilterHandler& filter);
    private:
        BlockBody root_;
        QString source_;
        FilterList filters_;
    };

}

#endif
