#ifndef LIQUID_CONTEXT_HPP
#define LIQUID_CONTEXT_HPP

#include "filter.hpp"

namespace Liquid {
    
    class Context {
    public:
        Context(Data& data, const FilterList& filters)
            : data_(data)
            , filters_(filters)
        {
        }
        
        const Data& data() const {
            return data_;
        }
        
        Data& data() {
            return data_;
        }
        
        const FilterList& filters() const {
            return filters_;
        }
        
        Data::Hash& environments() {
            return environments_;
        }
        
        Data::Hash& registers() {
            return registers_;
        }
        
        enum class Interrupt {
            Break,
            Continue,
        };
        
        bool haveInterrupts() const {
            return !interrupts_.empty();
        }
        
        void push_interrupt(Interrupt interrupt) {
            interrupts_.push_back(interrupt);
        }
        
        Interrupt pop_interrupt() {
            if (interrupts_.empty()) {
                throw std::string("Can't pop interrupts when empty");
            }
            const Interrupt i = interrupts_.back();
            interrupts_.pop_back();
            return i;
        }
        
    private:
        Data& data_;
        Data::Hash environments_;
        Data::Hash registers_;
        const FilterList& filters_;
        std::vector<Interrupt> interrupts_;
    };

}

#endif
