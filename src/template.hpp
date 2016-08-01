#ifndef LIQUID_TEMPLATE_HPP
#define LIQUID_TEMPLATE_HPP

#include <QString>

namespace Liquid {

    class Template {
    public:
        Template& parse(const QString& source);
    };

}

#endif
