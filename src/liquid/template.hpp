#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include "blockbody.hpp"
#include "filter.hpp"
#include "tag.hpp"

namespace Liquid {
    
    class Template {
    public:
        Template();
        
        Template& parse(const QString& source);
        
        QString render();
        QString render(Data& data);
        
        void registerFilter(const std::string& name, const FilterHandler& filter);
        
    private:
        BlockBody root_;
        QString source_;
        FilterList filters_;
        TagHash tags_;
    };

}

#endif
