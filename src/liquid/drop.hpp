#ifndef LIQUID_DROP_HPP
#define LIQUID_DROP_HPP

#include "string.hpp"
#include <functional>

namespace Liquid {
    
    class Data;

    class Drop {
    public:
        const Data& operator[](const String& key) const;
        
        bool operator==(const Drop& other) const;

    protected:
        virtual Data load(const String& key) const;

    private:
        mutable StringKeyUnorderedMap<Data> storage_;
    };
    
    class DropHandler : public Drop {
    public:
        using Loader = std::function<Data(const String& key)>;
        DropHandler(const Loader& loader);

    protected:
        virtual Data load(const String& key) const;
        
    private:
        Loader loader_;
    };
    
}

#endif
