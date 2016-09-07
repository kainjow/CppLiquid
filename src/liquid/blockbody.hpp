#ifndef LIQUID_BLOCKBODY_HPP
#define LIQUID_BLOCKBODY_HPP

#include <vector>
#include <functional>
#include <QString>
#include "node.hpp"

namespace Liquid {
    
    class Tokenizer;
    class Context;

    class BlockBody {
    public:
        using UnknownTagHandler = std::function<void(const QStringRef& tagName, Tokenizer& tokenizer)>;
        static void defaultUnknownTagHandler(const QStringRef& tagName, Tokenizer& tokenizer);

        void parse(Tokenizer& tokenizer, const UnknownTagHandler unknownTagHandler = defaultUnknownTagHandler);
        
        QString render(Context& context);
        
    private:
        std::vector<NodePtr> nodes_;
    };
    
}

#endif
