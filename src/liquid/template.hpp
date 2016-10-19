#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include "blockbody.hpp"
#include "filter.hpp"
#include "tag.hpp"

namespace Liquid {
    
    class Template {
    public:
        Template();
        
        Template& parse(const String& source);
        
        String render();
        String render(Data& data);
        
        void registerFilter(const std::string& name, const FilterHandler& filter);
        
    private:
        BlockBody root_;
        String source_;
        FilterList filters_;
        TagHash tags_;
    };

}

#endif
