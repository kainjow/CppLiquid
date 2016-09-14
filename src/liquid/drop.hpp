#ifndef LIQUID_DROP_HPP
#define LIQUID_DROP_HPP

#include "qstringhash.hpp"

namespace Liquid {
    
    class Data;

    class Drop {
    public:
        const Data& operator[](const QString& key) const;
        
        bool operator==(const Drop& other) const;

    protected:
        virtual Data load(const QString& key) const;

    private:
        mutable std::unordered_map<QString, Data, QStringHash> cache_;
    };
    
}

#endif
