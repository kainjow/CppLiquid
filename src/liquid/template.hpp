#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include "data.hpp"
#include "component.hpp"
#include "filter.hpp"

namespace Liquid {

    class Template {
    public:
        Template();
        
        void parse(const QString& source);
        
        QString render();
        QString render(const Data& data);
        
        void registerFilter(const std::string& name, const FilterHandler& filter);
    private:
        using ComponentPtr = std::unique_ptr<Component>;

        std::vector<ComponentPtr> components_;
        QString source_;
        FilterList filters_;
        
        std::vector<ComponentPtr> tokenize(const QString& source) const;
    };

}

#endif
