#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include "context.hpp"
#include "component.hpp"
#include <vector>

namespace Liquid {

    class Template {
    public:
        void parse(const QString& source);
        
        QString render();
        QString render(const Context& ctx);
        
    private:
        std::vector<Component> components_;
        QString source_;
        
        std::vector<Component> tokenize(const QString& source) const;
    };

}

#endif
