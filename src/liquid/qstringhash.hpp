#ifndef LIQUID_QSTRINGHASH_HPP
#define LIQUID_QSTRINGHASH_HPP

#include <unordered_map>
#include <QHashFunctions>
#include <QString>

namespace Liquid {
    
    struct QStringHash {
        std::size_t operator()(const QString& k) const {
            return qHash(k);
        }
    };

}

#endif
