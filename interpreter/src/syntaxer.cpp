#include <syntaxer.h>
#include <token.h>
#include <stdexcept>
#include <math.h>
namespace InterpreterNP{
CSyntaxer::CSyntaxer():_cur_tok_ind(0){}
CSyntaxer::CSyntaxer(const TokensArray& tok_arr):_cur_tok_ind(0){
    _tokens = tok_arr;
    _errors = {"Syntax error",
              "Non pair '('",
              "This is not expression",
              "There is meaning symbol '='",
              "Not variable",
              "Undefined function",
          "There is meaning symbol ']'"
             };
}
void CSyntaxer::SetTokBuf(const TokensArray& tok_arr){
    _tokens = tok_arr;
}
void CSyntaxer::SetResWords(const std::string& name,const std::function<void()>& val){
    _res_words[name] = val;
}
void CSyntaxer::SetVariables(const std::string& name, const CTokenValue& val){
    _variables[name] = val;
}
void CSyntaxer::SetFunction(const std::string& name, const std::function<CTokenValue(const CTokenValue&)>& val) {
  _functions[name] = val;
}
void CSyntaxer::SetSkipingDeviders(const std::unordered_set<std::string>& val){
    _skiped_dev = val;
}
void CSyntaxer::SetSequencePointDevider(const std::unordered_set<std::string>& val){
    _seq_p_dev = val;
}
void CSyntaxer::Run(){
    do {
        GetToken();
        if(_cur_tok.Type() == ttVariable) {
            PutBack(); 
            Assignment(); 
        }
        else if(_cur_tok.Type() == ttResWord){
            try{
              _res_words.at(_cur_tok.Text())();
            }
            catch(std::out_of_range&){
              throw std::runtime_error( GetCurError(5).c_str());
            }
        }
    } while (_cur_tok.Type() != ttFinish);
}
CToken CSyntaxer::GetToken(){
    if(!_cycle_stack.empty() && _cur_tok_ind == _cycle_stack.top().second){
        _cur_tok_ind = _cycle_stack.top().first;
    }
    CToken unk_tok;
    _cur_tok = _cur_tok_ind < _tokens.size() ? _tokens[_cur_tok_ind++] : unk_tok;

    return _cur_tok;
}
CTokenValue CSyntaxer::GetVariableByName(const std::string& name) const{
    return _variables.at(name);
}
void CSyntaxer::GetExp(CTokenValue& result){
    GetToken();
    SkipDevider();
    if(_cur_tok.Type() == ttUnknown){
        throw std::runtime_error( GetCurError(2).c_str());
    }
    Level_0(result);
    PutBack(); /* return last readed lexem in stream */
}
std::string CSyntaxer::GetCurError(size_t ind){
   return _errors[ind];
}
void CSyntaxer::PutBack(){
   --_cur_tok_ind;
}
void CSyntaxer::SkipDevider(){
   while(_cur_tok.Type() == ttDevider && _skiped_dev.find(_cur_tok.Text()) != _skiped_dev.end())//(_cur_tok.Text() == " " || _cur_tok.Text() == "\n" || _cur_tok.Text() == "\r" || _cur_tok.Text() == "\t"))
       GetToken();
}
void CSyntaxer::Level_0(CTokenValue& result) {
    CTokenValue hold;
    SkipDevider();
    Level_1(result);
    SkipDevider();
    while (_cur_tok.Type() == ttOperator && (_cur_tok.Text() == "||")) {
        auto op = _cur_tok.Text();
        GetToken();
        SkipDevider();
        Level_1(hold);
        Arithmetic(op, result, hold);
    }
}
void CSyntaxer::Level_1(CTokenValue& result) {
    CTokenValue hold;
    SkipDevider();
    Level_2(result);
    SkipDevider();
    while (_cur_tok.Type() == ttOperator && (_cur_tok.Text() == "&&")) {
        auto op = _cur_tok.Text();
        GetToken();
        SkipDevider();
        Level_2(hold);
        Arithmetic(op, result, hold);
    }
}
void CSyntaxer::Level_2(CTokenValue& result) {
    CTokenValue hold;
    SkipDevider();
    Level_3(result);
    SkipDevider();
    while (_cur_tok.Type() == ttOperator && (_cur_tok.Text() == "==" || _cur_tok.Text() == "!=")) {
        auto op = _cur_tok.Text();
        GetToken();
        SkipDevider();
        Level_3(hold);
        Arithmetic(op, result, hold);
    }
}
void CSyntaxer::Level_3(CTokenValue& result) {
    CTokenValue hold;
    SkipDevider();
    Level_4(result);
    SkipDevider();
    while (_cur_tok.Type() == ttOperator && (_cur_tok.Text() == ">" || _cur_tok.Text() == "<" || _cur_tok.Text() == ">=" || _cur_tok.Text() == "<=")) {
        auto op = _cur_tok.Text();
        GetToken();
        SkipDevider();
        Level_4(hold);
        Arithmetic(op, result, hold);
    }
}
void CSyntaxer::Level_4(CTokenValue& result){ 
    CTokenValue hold;
    SkipDevider();
    Level_5(result);
    SkipDevider();
    while(_cur_tok.Type() == ttOperator && (_cur_tok.Text() == "+" || _cur_tok.Text() == "-")) {
        auto op = _cur_tok.Text();
        GetToken();
        SkipDevider();
        Level_5(hold);
        Arithmetic(op,result,hold);
    }
}
void CSyntaxer::Level_5(CTokenValue& result){
     CTokenValue hold;
     SkipDevider();
     Level_6(result);
     SkipDevider();
     while(_cur_tok.Type() == ttOperator && (_cur_tok.Text() == "*" || _cur_tok.Text() == "/" || _cur_tok.Text() == "%")) {
                 auto op = _cur_tok.Text();
                 GetToken();
                 SkipDevider();
                 Level_6(hold);
                 Arithmetic(op,result,hold);
     }
}
void CSyntaxer::Level_6(CTokenValue& result){
     CTokenValue hold;
     SkipDevider();
     Level_7(result);
     SkipDevider();
     if(_cur_tok.Type() == ttOperator && _cur_tok.Text() == "^") {
         auto op = _cur_tok.Text();
         GetToken();
         SkipDevider();
         Level_7(hold);
         Arithmetic(op, result, hold);
     }
}

void CSyntaxer::Level_7(CTokenValue& result){
    register char op;
    op = 0;
    SkipDevider();
    if((_cur_tok.Type() == ttOperator) && (_cur_tok.Text() == "+" || _cur_tok.Text() == "-")) {
                 op = _cur_tok.Text()[0];
                 GetToken();
    }
    SkipDevider();
    Level_8(result);
    if(op)
     Unary(op, result);
}

void CSyntaxer::Level_8(CTokenValue& result){
    if((_cur_tok.Text() == "(") && (_cur_tok.Type() == ttDevider)) {
        GetToken();
        Level_0(result);
        if(_cur_tok.Text() != ")")
        throw std::runtime_error( GetCurError(1).c_str());
        GetToken();
    }
  else if (_cur_tok.Type() == ttFunction) {
    auto func = _functions.at(_cur_tok.Text());
    GetToken();
    SkipDevider();
    if (_cur_tok.Text() != "(")
      throw std::runtime_error(GetCurError(1).c_str());
    GetToken();
    SkipDevider();
    Level_0(result);
    if (_cur_tok.Text() != ")")
      throw std::runtime_error(GetCurError(1).c_str());
    result = func(result);
    GetToken();
  }
    else
        Primitive(result);
}

void CSyntaxer::Primitive(CTokenValue& result){
    switch(_cur_tok.Type()) {
      case ttVariable:
        result = FindVar(_cur_tok.Text());
        GetToken();
    SkipDevider();
    if (_cur_tok.Text() == "[") {
      CTokenValue index;
      GetExp(index);
      GetToken();
      SkipDevider();
      if (_cur_tok.Text() != "]") {
        throw std::runtime_error(GetCurError(6).c_str());
      }
      GetToken();
      SkipDevider();
      result = result[index.asString()];
    }
        return;
      case ttIntConstant:
          result.SetValue(_cur_tok.Text());
          result.SetType(tvInt);
        GetToken();
        return;
      case ttDoubleConstant:
          result.SetValue(_cur_tok.Text());
          result.SetType(tvDouble);
        GetToken();
        return;
      case ttStrConstant:
          result.SetValue(_cur_tok.Text());
          result.SetType(tvString);
        GetToken();
        return;
      default:
        throw std::runtime_error( GetCurError(0).c_str());
    }
}
CTokenValue CSyntaxer::FindVar(const std::string& s){
    if(_variables.find(s) == _variables.end())
        return CTokenValue();
    return _variables[s];
}
void CSyntaxer::Assignment(){
    std::string var;
    CTokenValue value, index;
  bool is_array = false;
    /* get variable name */
    GetToken();
    if(_cur_tok.Type() != ttVariable){
     throw std::runtime_error( GetCurError(4).c_str());
    }
    var = _cur_tok.Text();
    /* get symbol of equal*/
    GetToken();
    SkipDevider();
    
    if(_cur_tok.Text() != "=" && _cur_tok.Text() != "[" && _seq_p_dev.find(_cur_tok.Text()) == _seq_p_dev.end()) {
      throw std::runtime_error( GetCurError(3).c_str());
    }
  if (_cur_tok.Text() == "[") {
    GetExp(index);
    if (_variables.find(var) == _variables.end()) {
      CTokenValue array_val("", E_TOKEN_VALUE_TYPES::tvArray);
      _variables[var] = array_val;
    }
    is_array = true;
    GetToken();
    SkipDevider();
  }

  if (is_array && _cur_tok.Text() != "]") {
    throw std::runtime_error(GetCurError(6).c_str());
  }
  else if (is_array) {
    GetToken();
    SkipDevider();
  }
  if (_cur_tok.Text() != "=" && _seq_p_dev.find(_cur_tok.Text()) == _seq_p_dev.end()) {
    throw std::runtime_error(GetCurError(3).c_str());
  }
    /* get expression value */
    if(_cur_tok.Text() == "=")
      GetExp(value);
    /* set value*/
  if (is_array) {
    _variables[var][index.asString()] = value;
  }
  else {
    _variables[var] = value;
  }
}

void CSyntaxer::Arithmetic(const std::string& o,CTokenValue& r,CTokenValue& h){
    register int t;
    if (o == "-") {
        r = r - h;
        return;
    }
    if (o == "+") {
        r = r + h;
        return;
    }
    if (o == "*") {
        r = r * h;
        return;
    }
    if (o == "/") {
        r = r / h;
        return;
    }
    if (o == "&&") {
      if (r.GetType() == tvString || h.GetType() == tvString)
        throw std::runtime_error("bad operation for string");
      r.SetValue(std::to_string(r.asInt() && h.asInt()));
      r.SetType(tvInt);
      return;
    }
    if (o == "||") {
      if (r.GetType() == tvString || h.GetType() == tvString)
        throw std::runtime_error("bad operation for string");
      r.SetValue(std::to_string(r.asInt() || h.asInt()));
      r.SetType(tvInt);
      return;
    }
    if (o == "<") {
      r.SetValue(std::to_string(r < h));
      return;
    }
    if (o == ">") {
      r.SetValue(std::to_string(r > h));
      return;
    }
    if (o == "<=") {
      r.SetValue(std::to_string((r < h)||(r == h)));
      return;
    }
    if (o == ">=") {
      r.SetValue(std::to_string((r > h) || (r == h)));
      return;
    }
    if (o == "==") {
      r.SetValue(std::to_string(r == h));
      return;
    }
    if (o == "!=") {
      r.SetValue(std::to_string(!(r == h)));
      return;
    }
    if (o == "%") {
      if (r.GetType() == tvString || h.GetType() == tvString)
        throw std::runtime_error("bad operation for string");
      t = (r).asInt() / (h).asInt();
      r.SetValue(std::to_string(r.asInt() - (t*(h).asInt())));
      r.SetType(tvInt);
      return;
    }
    if (o == "^") {
        if (r.GetType() == tvString || h.GetType() == tvString) {
            throw std::runtime_error("bad operation for string");
        }
        r.SetType(tvDouble);
        r.SetValue(std::to_string(pow(r.asDouble(),h.asDouble())));
        return;
    }
}
void CSyntaxer::Unary(char o, CTokenValue& result){
    if(result.GetType() == tvString)
        throw std::runtime_error("incorrect operation for string");
    if(o=='-') {
        if(result.GetType() == tvDouble)
            result.SetValue(std::to_string(-(result.asDouble())));
        if(result.GetType() == tvInt)
            result.SetValue(std::to_string(-(result.asInt())));
    }
}
//work with token
E_TOKEN_TYPES CSyntaxer::GetCurTokType() {
  return _cur_tok.Type();
}
size_t CSyntaxer::GetCurTokIndex(){
    return _cur_tok_ind;
}
std::string CSyntaxer::GetCurTokText(){
    return _cur_tok.Text();
}
std::string CSyntaxer::GetTokByIndexText(size_t index){
    return _tokens[index].Text();
}
//work with cycle stack
bool CSyntaxer::IsCycleStackEmpty(){
    return _cycle_stack.empty();
}
std::pair<int, int> CSyntaxer::GetCycleStackTop(){
    return _cycle_stack.top();
}
void CSyntaxer::PushCycleStack(const std::pair<int, int>& val){
    _cycle_stack.push(val);
}
void CSyntaxer::PopCycleStack(){
    _cycle_stack.pop();
}
}
