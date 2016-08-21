#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include "data.hpp"
#include "component.hpp"
#include <memory>
#include <vector>

namespace Liquid {

    class Template {
    public:
        void parse(const QString& source);
        
        QString render();
        QString render(const Data& ctx);
        
    private:
        using ComponentPtr = std::unique_ptr<Component>;

        std::vector<ComponentPtr> components_;
        QString source_;
        
        std::vector<ComponentPtr> tokenize(const QString& source) const;
    };

}

#endif
