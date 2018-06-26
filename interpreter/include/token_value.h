#pragma once
#include "token_value_types.h"
#include <string>
namespace InterpreterNP{
class CTokenValue{
public:
    CTokenValue();
    CTokenValue(const std::string&,E_TOKEN_VALUE_TYPES);
    CTokenValue(const CTokenValue&);
    CTokenValue& operator=(const CTokenValue&);
    void SetValue(const std::string&);
    void SetName(const std::string&);
    void SetType(E_TOKEN_VALUE_TYPES);
    E_TOKEN_VALUE_TYPES GetType();
    std::string GetName() const;
    std::string asString() const;
    int asInt() const;
    double asDouble() const;
    friend CTokenValue operator+(const CTokenValue&,const CTokenValue&);
    friend CTokenValue operator-(const CTokenValue&,const CTokenValue&);
    friend CTokenValue operator*(const CTokenValue&,const CTokenValue&);
    friend CTokenValue operator/(const CTokenValue&,const CTokenValue&);
    bool operator == (const CTokenValue&);
    bool operator < (const CTokenValue&);
    bool operator > (const CTokenValue&);
private:
    E_TOKEN_VALUE_TYPES _cur_type;
    std::string _value;
    std::string _name;
};
}