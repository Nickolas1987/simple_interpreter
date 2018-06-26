#include "token_value.h"
#include <stdexcept>

namespace InterpreterNP{
CTokenValue::CTokenValue():_cur_type(tvString){
}
CTokenValue::CTokenValue(const CTokenValue& val):_cur_type(val._cur_type),_value(val._value){
}
CTokenValue::CTokenValue(const std::string& val, E_TOKEN_VALUE_TYPES type):_cur_type(type),_value(val){
}
CTokenValue& CTokenValue::operator=(const CTokenValue& val){
    if(this != &val){
     _cur_type = val._cur_type;
     _value = val._value;
    }
    return *this;
}
void CTokenValue::SetValue(const std::string& val){
    _value = val;
}
void CTokenValue::SetName(const std::string& val){
    _name = val;
}
void CTokenValue::SetType(E_TOKEN_VALUE_TYPES val){
    _cur_type = val;
}
E_TOKEN_VALUE_TYPES CTokenValue::GetType(){
    return _cur_type;
}
std::string CTokenValue::GetName() const{
    return _name;
}
std::string CTokenValue::asString() const{
    return _value;
}
int CTokenValue::asInt() const{
    return atoi(_value.c_str());
}
double CTokenValue::asDouble() const{
    return atof(_value.c_str());
}
CTokenValue operator+(const CTokenValue& left,const CTokenValue& right){
    if(left._cur_type == tvString || right._cur_type == tvString){
        return CTokenValue(left.asString() + right.asString(), tvString);
    }
    else if(left._cur_type == tvDouble || right._cur_type == tvDouble){
        return CTokenValue(std::to_string(left.asDouble() + right.asDouble()),tvDouble);
    }

    return CTokenValue(std::to_string(left.asInt() + right.asInt()),tvInt);
}
CTokenValue operator-(const CTokenValue& left,const CTokenValue& right){
    if(left._cur_type == tvDouble || right._cur_type == tvDouble){
        return CTokenValue(std::to_string(left.asDouble() - right.asDouble()),tvDouble);
    }
    return CTokenValue(std::to_string(left.asInt() - right.asInt()),tvInt);
}
CTokenValue operator*(const CTokenValue& left,const CTokenValue& right){
    if(left._cur_type == tvDouble || right._cur_type == tvDouble){
        return CTokenValue(std::to_string(left.asDouble() * right.asDouble()),tvDouble);
    }
    return CTokenValue(std::to_string(left.asInt() * right.asInt()),tvInt);
}
CTokenValue operator/(const CTokenValue& left,const CTokenValue& right){
    if(left._cur_type == tvDouble || right._cur_type == tvDouble){
        double tmp = right.asDouble();
        if(!tmp)
            throw std::overflow_error("Divide by zero exception");
        return CTokenValue(std::to_string(left.asDouble() / tmp),tvDouble);
    }
    int tmp = right.asInt();
    if(!tmp)
            throw std::overflow_error("Divide by zero exception");
    return CTokenValue(std::to_string(left.asInt() / tmp),tvInt);
}
bool CTokenValue::operator==(const CTokenValue& val){
    return val._value == _value && val._cur_type == _cur_type;
}

bool CTokenValue::operator<(const CTokenValue& val){
    if(val._cur_type == _cur_type){
        if(_cur_type == tvInt)
            return val.asInt() > asInt();
        else if(_cur_type == tvDouble){
            return val.asDouble() > asDouble();
        }
        else
            return val.asString() > asString();
    }
    return false;
}
bool CTokenValue::operator>(const CTokenValue& val){
   return !((*this) == val) && !((*this) < val);  
}
}
