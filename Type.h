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
    Type() {};

    Type(std::string type, T value) {
        this->type = type;
        this->value = value;
    }


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
};

#endif //RUM_TYPE_H
