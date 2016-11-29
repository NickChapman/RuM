//
// Created by Nick Chapman on 11/28/16.
//

#ifndef RUM_BOOLEAN_COMPARISONS_H
#define RUM_BOOLEAN_COMPARISONS_H

#include "Type.h"

namespace BOOL {
// Boolean type comparison functions
    bool equals(const std::shared_ptr<const TypeStruct> type1, const std::shared_ptr<const TypeStruct> type2) {
        // Int - Int
        if (type1->activeType == 'I' && type2->activeType == 'I') {
            return type1->typeUnion.intType->getValue() == type2->typeUnion.intType->getValue();
        }
            // Int - Float
        else if (type1->activeType == 'I' && type2->activeType == 'F') {
            return type1->typeUnion.intType->getValue() == type2->typeUnion.floatType->getValue();
        }
            // Int - Bool
        else if (type1->activeType == 'I' && type2->activeType == 'B') {
            return type1->typeUnion.intType->getValue() == type2->typeUnion.boolType->getValue();
        }
            // Float - Float
        else if (type1->activeType == 'F' && type2->activeType == 'F') {
            return type1->typeUnion.floatType->getValue() == type2->typeUnion.floatType->getValue();
        }
            // Float - Int
        else if (type1->activeType == 'F' && type2->activeType == 'I') {
            return type1->typeUnion.floatType->getValue() == type2->typeUnion.intType->getValue();
        }
            // Bool - Bool
        else if (type1->activeType == 'B' && type2->activeType == 'B') {
            return type1->typeUnion.boolType->getValue() == type2->typeUnion.boolType->getValue();
        }
            // Bool - Int
        else if (type1->activeType == 'B' && type2->activeType == 'I') {
            return type1->typeUnion.boolType->getValue() == type2->typeUnion.intType->getValue();
        }
            // String - String
        else if (type1->activeType == 'S' && type2->activeType == 'S') {
            return type1->typeUnion.stringType->getValue() == type2->typeUnion.stringType->getValue();
        }
        else {
            std::string errorMessage = "Used '==' on non comparable types ";
            errorMessage += type1->activeTypeAsString() + " and " + type2->activeTypeAsString();
            throw std::runtime_error(errorMessage);
        }
    }

    bool lessThan(const std::shared_ptr<const TypeStruct> type1, const std::shared_ptr<const TypeStruct> type2) {
        // Int - Int
        if (type1->activeType == 'I' && type2->activeType == 'I') {
            return type1->typeUnion.intType->getValue() < type2->typeUnion.intType->getValue();
        }
            // Int - Float
        else if (type1->activeType == 'I' && type2->activeType == 'F') {
            return type1->typeUnion.intType->getValue() < type2->typeUnion.floatType->getValue();
        }
            // Float - Float
        else if (type1->activeType == 'F' && type2->activeType == 'F') {
            return type1->typeUnion.floatType->getValue() < type2->typeUnion.floatType->getValue();
        }
            // Float - Int
        else if (type1->activeType == 'F' && type2->activeType == 'I') {
            return type1->typeUnion.floatType->getValue() < type2->typeUnion.intType->getValue();
        }
        else {
            std::string errorMessage = "Used '<' on non comparable types ";
            errorMessage += type1->activeTypeAsString() + " and " + type2->activeTypeAsString();
            throw std::runtime_error(errorMessage);
        }
    }

    bool greaterThan(const std::shared_ptr<const TypeStruct> type1, const std::shared_ptr<const TypeStruct> type2) {
        // Int - Int
        if (type1->activeType == 'I' && type2->activeType == 'I') {
            return type1->typeUnion.intType->getValue() > type2->typeUnion.intType->getValue();
        }
            // Int - Float
        else if (type1->activeType == 'I' && type2->activeType == 'F') {
            return type1->typeUnion.intType->getValue() > type2->typeUnion.floatType->getValue();
        }
            // Float - Float
        else if (type1->activeType == 'F' && type2->activeType == 'F') {
            return type1->typeUnion.floatType->getValue() > type2->typeUnion.floatType->getValue();
        }
            // Float - Int
        else if (type1->activeType == 'F' && type2->activeType == 'I') {
            return type1->typeUnion.floatType->getValue() > type2->typeUnion.intType->getValue();
        }
        else {
            std::string errorMessage = "Used '>' on non comparable types ";
            errorMessage += type1->activeTypeAsString() + " and " + type2->activeTypeAsString();
            throw std::runtime_error(errorMessage);
        }
    }

    bool logicalAnd(const std::shared_ptr<const TypeStruct> type1, const std::shared_ptr<const TypeStruct> type2) {
        // Int - Int
        if (type1->activeType == 'I' && type2->activeType == 'I') {
            return type1->typeUnion.intType->getValue() && type2->typeUnion.intType->getValue();
        }
            // Int - Float
        else if (type1->activeType == 'I' && type2->activeType == 'F') {
            return type1->typeUnion.intType->getValue() && type2->typeUnion.floatType->getValue();
        }
            // Int - Bool
        else if (type1->activeType == 'I' && type2->activeType == 'B') {
            return type1->typeUnion.intType->getValue() && type2->typeUnion.boolType->getValue();
        }
            // Float - Float
        else if (type1->activeType == 'F' && type2->activeType == 'F') {
            return type1->typeUnion.floatType->getValue() && type2->typeUnion.floatType->getValue();
        }
            // Float - Int
        else if (type1->activeType == 'F' && type2->activeType == 'I') {
            return type1->typeUnion.floatType->getValue() && type2->typeUnion.intType->getValue();
        }
            // Bool - Bool
        else if (type1->activeType == 'B' && type2->activeType == 'B') {
            return type1->typeUnion.boolType->getValue() && type2->typeUnion.boolType->getValue();
        }
            // Bool - Int
        else if (type1->activeType == 'B' && type2->activeType == 'I') {
            return type1->typeUnion.boolType->getValue() && type2->typeUnion.intType->getValue();
        }
        else {
            std::string errorMessage = "Used '&&' on non comparable types ";
            errorMessage += type1->activeTypeAsString() + " and " + type2->activeTypeAsString();
            throw std::runtime_error(errorMessage);
        }
    }

    bool logicalOr(const std::shared_ptr<const TypeStruct> type1, const std::shared_ptr<const TypeStruct> type2) {
        // Int - Int
        if (type1->activeType == 'I' && type2->activeType == 'I') {
            return type1->typeUnion.intType->getValue() || type2->typeUnion.intType->getValue();
        }
            // Int - Float
        else if (type1->activeType == 'I' && type2->activeType == 'F') {
            return type1->typeUnion.intType->getValue() || type2->typeUnion.floatType->getValue();
        }
            // Int - Bool
        else if (type1->activeType == 'I' && type2->activeType == 'B') {
            return type1->typeUnion.intType->getValue() || type2->typeUnion.boolType->getValue();
        }
            // Float - Float
        else if (type1->activeType == 'F' && type2->activeType == 'F') {
            return type1->typeUnion.floatType->getValue() || type2->typeUnion.floatType->getValue();
        }
            // Float - Int
        else if (type1->activeType == 'F' && type2->activeType == 'I') {
            return type1->typeUnion.floatType->getValue() || type2->typeUnion.intType->getValue();
        }
            // Bool - Bool
        else if (type1->activeType == 'B' && type2->activeType == 'B') {
            return type1->typeUnion.boolType->getValue() || type2->typeUnion.boolType->getValue();
        }
            // Bool - Int
        else if (type1->activeType == 'B' && type2->activeType == 'I') {
            return type1->typeUnion.boolType->getValue() || type2->typeUnion.intType->getValue();
        }
        else {
            std::string errorMessage = "Used '||' on non comparable types ";
            errorMessage += type1->activeTypeAsString() + " and " + type2->activeTypeAsString();
            throw std::runtime_error(errorMessage);
        }
    }

    std::shared_ptr<TypeStruct> negate(const std::shared_ptr<TypeStruct> value) {
        switch (value->activeType) {
            case 'I':
                value->typeUnion.boolType->setValue(value->typeUnion.intType->getValue() == 0);
                value->activeType = 'B';
                break;
            case 'F':
                value->typeUnion.boolType->setValue(value->typeUnion.floatType->getValue() == 0.0);
                value->activeType = 'B';
                break;
            case 'B':
                value->typeUnion.boolType->setValue(!value->typeUnion.boolType->getValue());
                break;
            default:
                throw std::runtime_error("Used '!' on a type that is not logically invertable.");
        }
        return value;
    }
}

#endif //RUM_BOOLEAN_COMPARISONS_H
