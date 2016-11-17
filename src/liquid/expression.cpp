#include "expression.hpp"
#include "standardfilters.hpp"

Liquid::Expression Liquid::Expression::parse(Parser& parser)
{
    if (parser.look(Token::Type::EndOfString)) {
        return Type::Nil;
    } else if (parser.look(Token::Type::String)) {
        return parser.consume().toString();
    } else if (parser.look(Token::Type::NumberInt)) {
        return parser.consume().toInt();
    } else if (parser.look(Token::Type::NumberFloat)) {
        return parser.consume().toDouble();
    }
    
    Expression exp(Type::Lookup);
    for (;;) {
        const StringRef key = parser.consume(Token::Type::Id);
        Expression sub(Type::LookupKey);
        sub.setKey(key.toString());
        if (key == "size") {
            sub.setLookupKeyFilter(Expression::LookupKeyFilter::Size);
        } else if (key == "first") {
            sub.setLookupKeyFilter(Expression::LookupKeyFilter::First);
        } else if (key == "last") {
            sub.setLookupKeyFilter(Expression::LookupKeyFilter::Last);
        }
        exp.addLookup(sub);
        
        if (parser.look(Token::Type::Dot)) {
            (void)parser.consume();
            continue;
        }
        
        while (parser.look(Token::Type::OpenSquare)) {
            (void)parser.consume();
            Expression bracketSub(Type::LookupBracketKey);
            bracketSub.addLookup(Expression::parse(parser));
            exp.addLookup(bracketSub);
            (void)parser.consume(Token::Type::CloseSquare);
        }
        
        break;
    }
    
    // Literals
    if (exp.lookups().size() == 1 && exp.lookups()[0].isLookupKey()) {
        const String& key = exp.lookups()[0].key();
        if (key == "nil" || key == "null") {
            return Type::Nil;
        } else if (key == "true") {
            return Type::BooleanTrue;
        } else if (key == "false") {
            return Type::BooleanFalse;
        }
    }
    
    return exp;
}

const Liquid::Data& Liquid::Expression::evaluate(const Data& data) const
{
    if (isLookupKey()) {
        if (data.isHash() || data.isDrop()) {
            const Data& result = data[key()];
            if (!result.isNil()) {
                return result;
            }
        }
        switch (lookupKeyFilter()) {
            case LookupKeyFilter::None:
                break;
            case LookupKeyFilter::Size: {
                // Since this function returns a value by reference, and size is dynamic, we need to store it.
                const int sizeValue = static_cast<int>(StandardFilters::size_imp(data));
                const auto existingResult = filterResults_.find(sizeValue);
                if (existingResult != filterResults_.end()) {
                    return existingResult->second;
                }
                return filterResults_.insert(std::make_pair(sizeValue, sizeValue)).first->second;
            }
            case LookupKeyFilter::First:
                return StandardFilters::first_imp(data);
            case LookupKeyFilter::Last:
                return StandardFilters::last_imp(data);
        }
    } else if (isLookup() || isLookupBracketKey()) {
        const Data* currentCtx = &data;
        for (const auto& lookup : lookups()) {
            if (lookup.isLookupBracketKey()) {
                const Data& bracketResult = lookup.evaluate(data);
                if (bracketResult.isString() && currentCtx->isHash()) {
                    const Data& result = (*currentCtx)[bracketResult.toString()];
                    if (result.isNil()) {
                        return result;
                    }
                    currentCtx = &result;
                } else if (bracketResult.isNumberInt() && currentCtx->isArray()) {
                    const Data& result = currentCtx->at(bracketResult.toInt());
                    if (result.isNil()) {
                        return result;
                    }
                    currentCtx = &result;
                } else {
                    return kNilData;
                }
            } else {
                const Data& result = lookup.evaluate(*currentCtx);
                if (result.isNil()) {
                    return result;
                }
                currentCtx = &result;
            }
        }
        return *currentCtx;
    } else if (isString() || isNumber() || isNil()) {
        return var_;
    } else if (isBoolean()) {
        return toBool() ? kTrueData : kFalseData;
    } else {
        throw std::runtime_error(String("Can't evaluate expression %1").arg(typeString()).toStdString());
    }
    return kNilData;
}

Liquid::String Liquid::Expression::stringDescription() const
{
    switch (type_) {
        case Type::Nil:
            return "Nil";
        case Type::String:
        case Type::NumberInt:
        case Type::NumberFloat:
        case Type::BooleanTrue:
        case Type::BooleanFalse:
            return toString();
        case Type::Lookup: {
            String ret;
            for (const auto &lookup : lookups_) {
                ret += lookup.stringDescription();
            }
            return ret;
        }
        case Type::LookupKey:
            return key();
        case Type::LookupBracketKey:
            return String("[") + (lookups_.empty() ? "" : lookups_[0].stringDescription()) + String("]");
        default:
            throw std::runtime_error(String("Unimplemented stringDescription for type %1").arg(typeString()).toStdString());
    };
}



#ifdef TESTS

#include "catch.hpp"

TEST_CASE("Liquid::Expression") {
    
    SECTION("String") {
        Liquid::Expression c("test");
        CHECK(c.isString());
        CHECK(c.toString() == "test");
        Liquid::Expression c2 = "Hello World";
        CHECK(c2.isString());
    }
    
    SECTION("Boolean") {
        Liquid::Expression c1(true);
        CHECK(c1.isBoolean());
        CHECK(c1.toBool());
        Liquid::Expression c2(false);
        CHECK(c2.isBoolean());
        CHECK_FALSE(c2.toBool());
        Liquid::Expression c3 = true;
        CHECK(c3.isBoolean());
        CHECK(c3.toBool() == true);
        Liquid::Expression c4 = false;
        CHECK(c4.isBoolean());
        CHECK(c4.toBool() == false);
    }
    
    SECTION("Number") {
        Liquid::Expression c1(23);
        CHECK(c1.isNumber());
        CHECK(c1.toInt() == 23);
        Liquid::Expression c2(23.832);
        CHECK(c2.isNumber());
        CHECK(c2.toFloat() == 23.832);
        Liquid::Expression c3 = 85;
        CHECK(c3.isNumber());
        CHECK(c3.type() == Liquid::Expression::Type::NumberInt);
        Liquid::Expression c4 = 85.32;
        CHECK(c4.isNumber());
        CHECK(c4.type() == Liquid::Expression::Type::NumberFloat);
    }
    
    SECTION("Copy") {
        Liquid::Expression c1(98);
        Liquid::Expression c2 = c1;
        Liquid::Expression c3(c2);
        CHECK(c1.type() == Liquid::Expression::Type::NumberInt);
        CHECK(c1.type() == c2.type());
        CHECK(c2.type() == c3.type());
        CHECK(c1.toInt() == 98);
        CHECK(c2.toInt() == 98);
        CHECK(c3.toInt() == 98);
    }
    
    SECTION("Parse") {
        Liquid::String input = "true";
        Liquid::Expression exp = Liquid::Expression::parse(&input);
        CHECK(exp.toBool());
        input = "false";
        exp = Liquid::Expression::parse(&input);
        CHECK_FALSE(exp.toBool());
        input = "nil";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNil());
        input = "null";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNil());
        input = "";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNil());
        input = "32";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isInt());
        CHECK(exp.toInt() == 32);
        input = "32.49";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isFloat());
        CHECK(exp.toInt() == 32);
        CHECK(exp.toFloat() == 32.49);
        input = "-32";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isInt());
        CHECK(exp.toInt() == -32);
        CHECK(exp.toFloat() == -32);
        input = "-32.49";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isFloat());
        CHECK(exp.toInt() == -32);
        CHECK(exp.toFloat() == -32.49);
        input = "-32.";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isNumber());
        CHECK(exp.isFloat());
        CHECK(exp.toInt() == -32);
        CHECK(exp.toFloat() == -32);
        input = "\"hello\"";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isString());
        CHECK(exp.toString() == "hello");
        input = "'hello'";
        exp = Liquid::Expression::parse(&input);
        CHECK(exp.isString());
        CHECK(exp.toString() == "hello");
    }
    
    SECTION("LookupKey") {
        Liquid::String input = "name";
        Liquid::Expression exp = Liquid::Expression::parse(&input);
        REQUIRE(exp.isLookup());
        REQUIRE(exp.lookups().size() == 1);
        CHECK(exp.lookups()[0].isLookupKey());
        CHECK(exp.lookups()[0].key() == "name");
    }

    SECTION("MultipleLookupKey") {
        Liquid::String input = "first.second.third.fourth.fifth";
        Liquid::Expression exp = Liquid::Expression::parse(&input);
        REQUIRE(exp.isLookup());
        REQUIRE(exp.lookups().size() == 5);
        CHECK(exp.lookups()[0].isLookupKey());
        CHECK(exp.lookups()[0].key() == "first");
        CHECK(exp.lookups()[1].isLookupKey());
        CHECK(exp.lookups()[1].key() == "second");
        CHECK(exp.lookups()[2].isLookupKey());
        CHECK(exp.lookups()[2].key() == "third");
        CHECK(exp.lookups()[3].isLookupKey());
        CHECK(exp.lookups()[3].key() == "fourth");
        CHECK(exp.lookups()[4].isLookupKey());
        CHECK(exp.lookups()[4].key() == "fifth");
    }
    
}

#endif
