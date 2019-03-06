#define _CRT_SECURE_NO_WARNINGS
#include <interpreter.h>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <stdexcept>
using namespace std;
namespace InterpreterNP{
CInterpreter::CInterpreter(){}
bool CInterpreter::Init(const std::unordered_map<std::string, CTokenValue>& variables){
    string data[]={"if","while","print","input","exit",
                   "count",
                   "+", "-", "=", "/", "*", "^", ">", "<", "==", "!=", ">=", "<=", "~", "!", "||", "&&",
                   " ", ";", "(", ")", ",", "{", "}", "[", "]", "\n", "\t", "\r", "%"
                  };
    _lexer.Init(data, 5, 1, 16, 13);
    _syntaxer.SetResWords("if", std::bind(&CInterpreter::IfFunc,this));
    _syntaxer.SetResWords("while", std::bind(&CInterpreter::WhileFunc,this));
    _syntaxer.SetResWords("print", std::bind(&CInterpreter::PrintFunc,this));
    _syntaxer.SetResWords("input", std::bind(&CInterpreter::InputFunc,this));
    _syntaxer.SetResWords("exit", std::bind(&CInterpreter::ExitFunc, this));
    _syntaxer.SetFunction("count", [](const CTokenValue& val)->CTokenValue {
        return CTokenValue(std::to_string(val.size()), E_TOKEN_VALUE_TYPES::tvInt); 
    });
    _syntaxer.SetSkipingDeviders({" ", "\t", "\r", "\n"});
    _syntaxer.SetSequencePointDevider({";"});
    _variables = variables;
    if(!_variables.empty()){
      for(auto iter : _variables){
        _syntaxer.SetVariables(iter.first, iter.second);
      }
    }
    return true;
}
std::unordered_map<std::string, CTokenValue> CInterpreter::GetVariables() const{
    std::unordered_map<std::string, CTokenValue> res;
    for(auto iter : _variables){
      res.insert(std::make_pair(iter.first, _syntaxer.GetVariableByName(iter.first)));
    }
    return res;
}
bool CInterpreter::Run(const std::string& file_name){
    std::ifstream _file(file_name);
    if(!_file.is_open())
     return false;
    std::string _str((std::istreambuf_iterator<char>(_file)),
                 std::istreambuf_iterator<char>());
    _lexer.Lex(_str);
//    _lexer.SaveTokens(std::cout);
    _syntaxer.SetTokBuf(_lexer.GetTokens());
    _syntaxer.Run();

  return true;
}
void CInterpreter::ExitFunc() {
    while (_syntaxer.GetToken().Type() != ttFinish);
}
void CInterpreter::PrintFunc(){
    CToken tok;
    do {
       tok = _syntaxer.GetToken(); /* get next tok */
       if (tok.Type() == ttFinish || (tok.Type() == ttDevider && tok.Text() == ";")) 
        break;
       if (tok.Type() == ttDevider) 
        continue;
       if (tok.Type() == ttStrConstant || tok.Type() == ttIntConstant || tok.Type() == ttDoubleConstant ) { 
        std::cout << tok.Text();
        _syntaxer.GetToken();
       }
       else { /* if it is a expr */
        _syntaxer.PutBack();
        CTokenValue tok_val;
        _syntaxer.GetExp(tok_val);
        std::cout << tok_val.asString();
       }
       if(_syntaxer.GetCurTokText() == ";")
        break;
       if(_syntaxer.GetCurTokType() == ttFinish)
        break;
    } while (true);
}
void CInterpreter::InputFunc(){
    std::string name, value;
    CTokenValue input_value;
    _syntaxer.GetToken();
    _syntaxer.SkipDevider();
    if(_syntaxer.GetCurTokType() == ttStrConstant) {
     if(_syntaxer.GetCurTokText() == "%s")
           input_value.SetType(tvString);
     if(_syntaxer.GetCurTokText() == "%d")
           input_value.SetType(tvInt);
     if(_syntaxer.GetCurTokText() == "%f")
           input_value.SetType(tvDouble);
     _syntaxer.GetToken();
     _syntaxer.SkipDevider();
     if (_syntaxer.GetCurTokText() != ",") {
         throw runtime_error( _syntaxer.GetCurError(5).c_str());
         return;
     }
     _syntaxer.GetToken();
     _syntaxer.SkipDevider();
    }
    else{
     input_value.SetType(tvString);
    }
    name = _syntaxer.GetCurTokText();
    std::cin >> value;   /* read input data */
    input_value.SetValue(value);
    _syntaxer.SetVariables(name,input_value);  /* save input */
}
void CInterpreter::WhileFunc(){
     if(_syntaxer.IsCycleStackEmpty() || _syntaxer.GetCycleStackTop().first != _syntaxer.GetCurTokIndex() - 1)
                  _syntaxer.PushCycleStack(std::make_pair(_syntaxer.GetCurTokIndex() - 1, -1));
     bool cond = false;
     CTokenValue val_tok;
     _syntaxer.SkipDevider();

     if(_syntaxer.GetToken().Text() != "("){
        throw std::runtime_error("incorrect operator while syntax");
        return;
     }
     _syntaxer.PutBack();
     _syntaxer.GetExp(val_tok); 
     _syntaxer.SkipDevider();
     cond = static_cast<bool>(val_tok.asInt());

     if(cond){
         _syntaxer.GetToken();
         _syntaxer.SkipDevider();   //continue with new string
         if(_syntaxer.GetCurTokText() == "{"){
             int inside_cycle_pos = _syntaxer.GetCurTokIndex();
             while( _syntaxer.GetTokByIndexText(inside_cycle_pos++) != "}");
             std::pair<int,int> last_cycle = _syntaxer.GetCycleStackTop();
             if(last_cycle.second == -1){
              last_cycle.second = inside_cycle_pos - 1;
              _syntaxer.PopCycleStack();
              _syntaxer.PushCycleStack(last_cycle);
             }
         }
         else
            throw std::runtime_error("incorrect operator while syntax");
         return;
     }
     else{  
        _syntaxer.GetToken();
        _syntaxer.SkipDevider();
        _syntaxer.PopCycleStack();
        if(_syntaxer.GetCurTokText() == "{"){
            int count_open = 1, count_close = 0;
            while (true) {
                auto tok = _syntaxer.GetToken();
                if (tok.Type() == ttFinish) {
                    throw std::runtime_error("incorrect count }");
                }
                if (tok.Type() == ttDevider && tok.Text() == "{") {
                    ++count_open;
                }
                if (tok.Type() == ttDevider && tok.Text() == "}") {
                    ++count_close;
                }
                if (count_open == count_close) {
                    break;
                }
            }
        }
        else
            throw std::runtime_error("incorrect operator while syntax");
     }
}
void CInterpreter::IfFunc(){
     bool cond = false;
     CTokenValue val_tok;
     _syntaxer.SkipDevider();
     if(_syntaxer.GetToken().Text() != "("){
        throw runtime_error("incorrect operator if syntax");
     }
     _syntaxer.PutBack();
     _syntaxer.GetExp(val_tok);

     cond = static_cast<bool>(val_tok.asInt());
     if (cond) {
        return;
     }
     else {
        _syntaxer.GetToken();
        _syntaxer.SkipDevider();   /* If not true, start with str after '}'  */
        if (_syntaxer.GetCurTokText() == "{") {
            int count_open = 1, count_close = 0;
            while (true) {
                auto tok = _syntaxer.GetToken();
                if (tok.Type() == ttFinish) {
                    throw std::runtime_error("incorrect count }");
                }
                if (tok.Type() == ttDevider && tok.Text() == "{") {
                    ++count_open;
                }
                if (tok.Type() == ttDevider && tok.Text() == "}") {
                    ++count_close;
                }
                if (count_open == count_close) {
                    break;
                }
            }
        }
        else{
            while (true) {
                auto tok = _syntaxer.GetToken();
                if (tok.Type() == ttFinish) {
                    throw std::runtime_error("incorrect if end");
                }
                if (tok.Type() == ttDevider && tok.Text() == ";") {
                    break;
                }
            }
        }
     }

}
}
