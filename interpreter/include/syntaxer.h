#pragma once

#include <vector>
#include <unordered_map>
#include <functional>
#include <stack>
#include "lexer.h"
#include "log.h"
#include "token_value.h"
namespace InterpreterNP{
class CSyntaxer{
public:
    CSyntaxer();
    CSyntaxer(const TokensArray& tok_arr);
    void SetTokBuf(const TokensArray& tok_arr);
    void Run(); // run syntaxer
    void GetExp(CTokenValue& result); // get expression
    void Assignment(); // assign value to variable
    void PutBack(); // return token to stream
    void SkipDevider();
    CToken GetToken();
    std::string GetCurError(size_t ind);
    void SetResWords(const std::string&,const std::function<void()>&);
    void SetVariables(const std::string&, const CTokenValue&);
    //////work with cur token 
    E_TOKEN_TYPES GetCurTokType();
    std::string GetCurTokText();
    std::string GetTokByIndexText(size_t);
    size_t GetCurTokIndex();
    ///////work with cycle stack
    bool IsCycleStackEmpty();
    std::pair<int, int> GetCycleStackTop();
    void PushCycleStack(const std::pair<int, int>&);
    void PopCycleStack();
private:

    CSyntaxer(const CSyntaxer&);
    CSyntaxer& operator=(const CSyntaxer&);

    void Level_2(CTokenValue& result); // + and -
    void Level_3(CTokenValue& result); // * and /
    void Level_4(CTokenValue& result); // ^ operation
    void Level_5(CTokenValue& result); // unary -
    void Level_6(CTokenValue& result); // ()
    void Arithmetic(char o,CTokenValue& r,CTokenValue& h);
    void Primitive(CTokenValue& result);
    void Unary(char o, CTokenValue& result);
    CTokenValue FindVar(const std::string&);

    TokensArray _tokens;
    CToken _cur_tok;
    size_t _cur_tok_ind;
    std::vector<std::string> _errors;
    std::unordered_map<std::string, CTokenValue> _variables;
    std::unordered_map<std::string, std::function<void()> > _res_words;
    std::stack<std::pair<int,int> > _cycle_stack;
};
}
