#include <token_value.h>
#include <stdexcept>

namespace InterpreterNP{
CTokenValue::CTokenValue():_cur_type(tvString){
}
CTokenValue::CTokenValue(const CTokenValue& val):_cur_type(val._cur_type),_value(val._value), _values(val._values){
}
CTokenValue::CTokenValue(const std::string& val, E_TOKEN_VALUE_TYPES type):_cur_type(type),_value(val){
}
CTokenValue& CTokenValue::operator=(const CTokenValue& val){
    if(this != &val){
     _cur_type = val._cur_type;
     _value = val._value;
     _values = val._values;
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
    if (_cur_type == tvArray)
        throw std::runtime_error("bad cast array to string");
    return _value;
}
int CTokenValue::asInt() const{
    if (_cur_type == tvArray)
        throw std::runtime_error("bad cast array to int");
    return atoi(_value.c_str());
}
double CTokenValue::asDouble() const{
    if (_cur_type == tvArray)
        throw std::runtime_error("bad cast array to double");
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
    bool res = false;
    switch (_cur_type)
    {
        case tvInt:
            res = (this->asInt() == val.asInt());
            break;
        case tvDouble:
            res = (this->asDouble() == val.asDouble());
            break;
        case tvString:
            res = (this->_value == val._value);
            break;
        default:
            break;
    }
    return res;
}

bool CTokenValue::operator<(const CTokenValue& val){
   if (_cur_type == tvInt) {
      return val.asInt() > asInt();
   }
   else if(_cur_type == tvDouble){
      return val.asDouble() > asDouble();
   }
   else if (_cur_type == tvString) {
      return val.asString() > asString();
   }
   return false;
}
bool CTokenValue::operator>(const CTokenValue& val){
   return !((*this) == val) && !((*this) < val);  
}
CTokenValue& CTokenValue::operator[](const std::string& key) {
    if (_cur_type != tvArray)
        throw std::runtime_error("bad operation []");
    if(_values.find(key) == _values.end())
        _values[key] = std::make_shared<CTokenValue>();
    return *(_values.at(key).get());
}
CTokenValue& CTokenValue::operator[](int key) {
    if (_cur_type != tvArray)
        throw std::runtime_error("bad operation []");
    std::string key_str = std::to_string(key);
    if(_values.find(key_str) == _values.end())
        _values[key_str] = std::make_shared<CTokenValue>();
    return *(_values.at(key_str).get());
}
const CTokenValue& CTokenValue::operator[](const std::string& key) const {
    if (_cur_type != tvArray)
        throw std::runtime_error("bad operation []");
    return *(_values.at(key).get());
}
const CTokenValue& CTokenValue::operator[](int key) const {
    if (_cur_type != tvArray)
        throw std::runtime_error("bad operation []");
    std::string key_str = std::to_string(key);
    return *(_values.at(key_str).get());
}
size_t CTokenValue::GetSize() const {
    return _values.size();
}
}
