#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <stack>
#include <lexer.h>
#include <token_value.h>
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
    CTokenValue GetVariableByName(const std::string&) const;
    std::string GetCurError(size_t ind);
    void SetResWords(const std::string&,const std::function<void()>&);
    void SetFunction(const std::string&, const std::function<CTokenValue(const CTokenValue&)>&);
    void SetVariables(const std::string&, const CTokenValue&);
    void SetSkipingDeviders(const std::unordered_set<std::string>&);
    void SetSequencePointDevider(const std::unordered_set<std::string>&);
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

    CSyntaxer(const CSyntaxer&) = delete;
    CSyntaxer& operator=(const CSyntaxer&) = delete;

    void Level_0(CTokenValue& result); // ||
    void Level_1(CTokenValue& result); // &&
    void Level_2(CTokenValue& result); // >,<,<=,>=
    void Level_3(CTokenValue& result); // ==,!=
    void Level_4(CTokenValue& result); // + and -
    void Level_5(CTokenValue& result); // * and /
    void Level_6(CTokenValue& result); // ^ operation
    void Level_7(CTokenValue& result); // unary -
    void Level_8(CTokenValue& result); // () and func
    void Arithmetic(const std::string& o,CTokenValue& r,CTokenValue& h);
    void Primitive(CTokenValue& result);
    void Unary(char o, CTokenValue& result);
    CTokenValue FindVar(const std::string&);

    TokensArray _tokens;
    CToken _cur_tok;
    size_t _cur_tok_ind;
    std::vector<std::string> _errors;
    std::unordered_map<std::string, CTokenValue> _variables;
    std::unordered_map<std::string, std::function<void()> > _res_words;
    std::unordered_map<std::string, std::function<CTokenValue(const CTokenValue&)> > _functions;
    std::stack<std::pair<int,int> > _cycle_stack;
    std::unordered_set<std::string> _skiped_dev;
    std::unordered_set<std::string> _seq_p_dev;
};
}
