#define _CRT_SECURE_NO_WARNINGS
#include "interpreter.h"
#include <iostream>
using namespace std;
namespace InterpreterNP{
CInterpreter::CInterpreter(){}
bool CInterpreter::Init(){
    string data[]={"if","while","print","input",
               "+", "-", "=", "/", "*", ">", "<", "~", "!", "|", "&",
               " ", ";", "(", ")", ",", "{", "}", "\n", "\t", "\r", "%"};
    _lexer.Init(data,4,11,11);
    _syntaxer.SetResWords("if", std::bind(&CInterpreter::IfFunc,this));
    _syntaxer.SetResWords("while", std::bind(&CInterpreter::WhileFunc,this));
    _syntaxer.SetResWords("print", std::bind(&CInterpreter::PrintFunc,this));
    _syntaxer.SetResWords("input", std::bind(&CInterpreter::InputFunc,this));
    return true;
}
bool CInterpreter::Run(const std::string& file_name){
    FILE *f = fopen(file_name.c_str(),"rt");
    if(!f)
        return false;
    fseek(f, 0, SEEK_END); long len = ftell(f); fseek(f, 0, SEEK_SET);
    string str;
    str.resize(len);
    fread(&str[0],1,len,f);
    fclose(f);

    _lexer.Lex(str);
    _syntaxer.SetTokBuf(_lexer.GetTokens());
    _syntaxer.Run();

  return true;
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
         ESLLog.WriteError( _syntaxer.GetCurError(5).c_str());
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
     std::string op;
     CTokenValue val_left, val_right;
     _syntaxer.SkipDevider();

     if(_syntaxer.GetToken().Text() != "("){
        throw std::runtime_error("incorrect operator while syntax");
        return;
     }
     _syntaxer.GetExp(val_left); /* get left expr */
     _syntaxer.SkipDevider();

     op = _syntaxer.GetToken().Text(); /* get operator  */
     if(op[0] != ')'){
       _syntaxer.SkipDevider();
       _syntaxer.GetExp(val_right);  /* get right expr */
         /* get result */
       _syntaxer.SkipDevider();

       if(_syntaxer.GetToken().Text() != ")"){
        throw std::runtime_error("incorrect operator while syntax");
        return;
       }
     }
     switch(op[0]) {
       case '=':
         if(val_left==val_right) cond=true;
           break;
       case '<':
           if(val_left < val_right) cond = true;
            break;
       case '>':
           if(val_left > val_right) cond=true;
           break;
       case ')':
           if(val_left.asInt()) cond = true;
           break;
     }

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
            while(_syntaxer.GetToken().Text() != "}");
        }
        else
            throw std::runtime_error("incorrect operator while syntax");
     }
}
void CInterpreter::IfFunc(){
     bool cond = false;
     std::string op;
     CTokenValue val_left, val_right;
     _syntaxer.SkipDevider();
     if(_syntaxer.GetToken().Text() != "("){
        ESLLog.WriteError("incorrect operator if syntax");
        return;
     }
     _syntaxer.GetExp(val_left); /* get left exp */
     _syntaxer.SkipDevider();
     op = _syntaxer.GetToken().Text(); /* operator  */
     if(op[0] != ')'){
       _syntaxer.SkipDevider();
       _syntaxer.GetExp(val_right);  /* right exp */
         /* result */
       _syntaxer.SkipDevider();
       if(_syntaxer.GetToken().Text() != ")"){
        ESLLog.WriteError("incorrect operator if syntax");
        return;
       }
     }
     cond=0;
     switch(op[0]) {
       case '=':
         if(val_left == val_right) cond=true;
           break;
       case '<':
           if(val_left < val_right) cond = true;
            break;
       case '>':
           if(val_left > val_right) cond=true;
           break;
       case ')':
           if(val_left.asInt()) cond = true;
           break;
     }
     if(cond) 
           return;
     else{  
        _syntaxer.GetToken();
        _syntaxer.SkipDevider();   /* If not true, start with str after '}'  */
        if(_syntaxer.GetCurTokText() == "{"){
            while(_syntaxer.GetToken().Text() != "}");
        }
        else
            while (_syntaxer.GetToken().Text() != ";");
     }
}
}