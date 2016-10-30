#ifndef LIQUID_STRING_QT_HPP
#define LIQUID_STRING_QT_HPP

#ifndef LIQUID_STRING_USE_QT
#error "LIQUID_STRING_USE_QT must be defined to use this file!"
#endif

#include <QString>
#include <QHashFunctions>
#include <unordered_map>

namespace Liquid {

    using String = QString;

    struct QStringHash {
        std::size_t operator()(const String& k) const {
            return qHash(k);
        }
    };
    
    template <typename T>
    using StringKeyUnorderedMap = std::unordered_map<String, T, QStringHash>;
    
};

#endif
