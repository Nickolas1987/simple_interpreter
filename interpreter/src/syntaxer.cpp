#include "syntaxer.h"
#include "token.h"
#include <iostream>
namespace InterpreterNP{
CSyntaxer::CSyntaxer():_cur_tok_ind(0){}
CSyntaxer::CSyntaxer(const TokensArray& tok_arr):_cur_tok_ind(0){
    _tokens = tok_arr;
    _errors = {"Syntax error",
              "Non pair '('",
              "This is not expression",
              "There is meaning symbol '='",
              "Not variable",
              "Undefined function"
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
void CSyntaxer::SetSkipingDeviders(const std::unordered_set<std::string>& val){
    _skiped_dev = val;
}
void CSyntaxer::SetSequencePointDevider(const std::unordered_set<std::string>& val){
    _seq_p_dev = val;
}
void CSyntaxer::Run(){
    do {
        GetToken();
        //std::cout << "Cur tok :" << _cur_tok.Text() <<std::endl;
        if(_cur_tok.Type() == ttVariable) {
            PutBack(); 
            Assignment(); 
        }
        else if(_cur_tok.Type() == ttResWord){
            try{
              _res_words.at(_cur_tok.Text())();
            }
            catch(std::out_of_range&){
              ESLLog.WriteError( GetCurError(5).c_str());
            }
        }
        else if(_cur_tok.Type() == ttFunction){ /* this is command */
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

void CSyntaxer::GetExp(CTokenValue& result){
  try{
    GetToken();
    if(_cur_tok.Type() == ttUnknown){
        ESLLog.WriteError( GetCurError(2).c_str());
       return;
    }
    Level_2(result);
    PutBack(); /* return last readed lexem in stream */
  }
  catch(std::runtime_error& e){
    ESLLog.WriteError(e.what());
  }
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
void CSyntaxer::Level_2(CTokenValue& result){ 
    CTokenValue hold;
    SkipDevider();
    Level_3(result);
    SkipDevider();
    while(_cur_tok.Type() == ttOperator && (_cur_tok.Text() == "+" || _cur_tok.Text() == "-")) {
        char op = _cur_tok.Text()[0];
        GetToken();
        SkipDevider();
        Level_3(hold);
        Arithmetic(op,result,hold);
    }
}
void CSyntaxer::Level_3(CTokenValue& result){
     CTokenValue hold;
     SkipDevider();
     Level_4(result);
     SkipDevider();
     while(_cur_tok.Type() == ttOperator && (_cur_tok.Text() == "*" || _cur_tok.Text() == "/" || _cur_tok.Text() == "%")) {
                 char op = _cur_tok.Text()[0];
                 GetToken();
                 SkipDevider();
                 Level_4(hold);
                 Arithmetic(op,result,hold);
     }
}
void CSyntaxer::Level_4(CTokenValue& result){
     CTokenValue hold;
     SkipDevider();
     Level_5(result);
     SkipDevider();
     if(_cur_tok.Type() == ttOperator && _cur_tok.Text() == "^") {
         char op = _cur_tok.Text()[0];
         GetToken();
         SkipDevider();
         Level_6(hold);
         Arithmetic(op, result, hold);
     }
}

void CSyntaxer::Level_5(CTokenValue& result){
    register char op;
    op = 0;
    SkipDevider();
    if((_cur_tok.Type() == ttOperator) && (_cur_tok.Text() == "+" || _cur_tok.Text() == "-")) {
                 op = _cur_tok.Text()[0];
                 GetToken();
    }
    SkipDevider();
    Level_6(result);
    if(op)
     Unary(op, result);
}

void CSyntaxer::Level_6(CTokenValue& result){
    if((_cur_tok.Text() == "(") && (_cur_tok.Type() == ttOperator)) {
        GetToken();
        Level_2(result);
        if(_cur_tok.Text() != ")")
        ESLLog.WriteError( GetCurError(1).c_str());
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
        ESLLog.WriteError( GetCurError(0).c_str());
    }
}
CTokenValue CSyntaxer::FindVar(const std::string& s){
    if(_variables.find(s) == _variables.end())
        return CTokenValue();
    return _variables[s];
}
void CSyntaxer::Assignment(){
    std::string var;
    CTokenValue value;
    /* get variable name */
    GetToken();
    if(_cur_tok.Type() != ttVariable){
     ESLLog.WriteError( GetCurError(4).c_str());
     return;
    }
    var = _cur_tok.Text();
    /* get symbol of equal*/
    GetToken();
    SkipDevider();
    
     if(_cur_tok.Text() != "=" && _seq_p_dev.find(_cur_tok.Text()) == _seq_p_dev.end()) {
      ESLLog.WriteError( GetCurError(3).c_str());
      return;
     }
    
    /* get expression value */
    if(_cur_tok.Text() == "=")
      GetExp(value);
    /* set value*/
    _variables[var] = value;
}

void CSyntaxer::Arithmetic(char o,CTokenValue& r,CTokenValue& h){
    register int t, ex;
    switch(o) {
      case '-':
        r = r - h;
        break;
      case '+':
        r = r + h;
        break;
      case '*':
        r = r * h;
        break;
      case '/':
        r = (r) / (h);
        break;
      case '%':
        if(r.GetType() == tvString || h.GetType() == tvString)
          throw std::runtime_error("bad operation for string");
          t = (r).asInt() / (h).asInt();
          r.SetValue(std::to_string(r.asInt()-(t*(h).asInt())));
          r.SetType(tvInt);
        break;
      case '^':
        if(r.GetType() == tvString || h.GetType() == tvString)
          throw std::runtime_error("bad operation for string");
          ex = r.asInt();
        if(h.asInt()==0) {
          r.SetValue("1");
          break;
        }
        for(t = h.asInt()-1; t > 0; --t) ex = (r).asInt() * ex;
          r.SetType(tvInt);
          r.SetValue(std::to_string(ex));
          break;
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
