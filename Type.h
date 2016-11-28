//
// Created by Nick Chapman on 11/20/16.
//

#ifndef RUM_TYPE_H
#define RUM_TYPE_H

#include <string>

template<class T>
class Type {
    std::string identifier;
    std::string type;
    T value;
public:
    Type() {
        identifier = "";
        type = "";
    };

    Type(std::string type, T value) {
        this->type = type;
        this->value = value;
    }

    ~Type() {}

    std::string getType() const {
        return this->type;
    }

    void setType(std::string type) {
        this->type = type;
    }

    void setValue(T value) {
        this->value = value;
    }

    T getValue() const {
        return this->value;
    }

    Type<T>* getDeepCopy() {
        return new Type<T>(this->getType(), this->getValue());
    }
};

struct TypeStruct {
    union type_union {
        Type<int> *intType;
        Type<float> *floatType;
        Type<std::string> *stringType;
        Type<bool> *boolType;

        type_union() {
            intType = nullptr;
        }

        ~type_union() {
            delete intType;
        }
    };

    type_union typeUnion;
    char activeType;

    TypeStruct() {
        typeUnion = type_union();
        activeType = 'N';
    }

    TypeStruct(std::shared_ptr<TypeStruct> other) {
        this->activeType = other->activeType;
        this->typeUnion = type_union();
        switch (this->activeType) {
            case 'I':
                this->typeUnion.intType = other->typeUnion.intType->getDeepCopy();
                break;
            case 'F':
                this->typeUnion.floatType = other->typeUnion.floatType->getDeepCopy();
                break;
            case 'B':
                this->typeUnion.boolType = other->typeUnion.boolType->getDeepCopy();
                break;
            case 'S':
                this->typeUnion.stringType = other->typeUnion.stringType->getDeepCopy();
                break;
            default:
                this->typeUnion.intType = nullptr;
        }
    }

    const std::string activeTypeAsString() const {
        switch(this->activeType) {
            case 'I':
                return "integer";
            case 'F':
                return "float";
            case 'B':
                return "boolean";
            case 'S':
                return "string";
            default:
                return "nullptr";
        }
    }

    ~TypeStruct() {}
};

#endif //RUM_TYPE_H
