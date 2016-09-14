#ifndef LIQUID_DROP_HPP
#define LIQUID_DROP_HPP

#include "qstringhash.hpp"
#include <functional>

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
    
    class DropHandler : public Drop {
    public:
        using Loader = std::function<Data(const QString& key)>;
        DropHandler(const Loader& loader);

    protected:
        virtual Data load(const QString& key) const;
        
    private:
        Loader loader_;
    };
    
}

#endif
