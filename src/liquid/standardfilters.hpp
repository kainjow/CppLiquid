#ifndef LIQUID_STANDARDFILTERS_HPP
#define LIQUID_STANDARDFILTERS_HPP

#include "template.hpp"

namespace Liquid { namespace StandardFilters {

    void registerFilters(Template& tmpl);

    size_t size_imp(const Data& input);
    const Data& first_imp(const Data& input);
    const Data& last_imp(const Data& input);

} }

#endif
