#pragma once

#include <vector>
#include <iostream>
#include <unordered_set>
#include <token.h>

namespace InterpreterNP{
typedef std::vector<CToken> TokensArray;
 
class CLexer{
public:
    const TokensArray& GetTokens(void)const;
    void  SaveTokens(std::ostream& os)const;
    bool  Lex(const std::string&); // "lexing" text
    void  Init(const std::string*, unsigned, unsigned, unsigned, unsigned);
private:
    std::unordered_set<std::string>  _ResWordsArray; // reserved words
    std::unordered_set<std::string>  _FuncArray;
    std::unordered_set<std::string>               _Operators, 
                                                   _Deviders;
    TokensArray               _TokensBuffer; // tokens after last "lexing"
    unsigned                  _OffSet; // current token position
    CToken  SkanToken(const std::string&);
    void  DefineTokenType(CToken&)const;
    CToken SkanStringConstant(const std::string& text);
    void SkipComment(const std::string& text);
    void SkipSpacing(const std::string& text);
};
}
