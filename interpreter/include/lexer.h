#pragma once

#include <vector>
#include <iostream>
#include "token.h"

namespace InterpreterNP{
typedef std::vector<CToken> TokensArray;
 
class CLexer{
public:
    const TokensArray& GetTokens(void)const {return _TokensBuffer;}
    void  SaveTokens(std::ostream& os)const;
    bool  Lex(const std::string&); // "lexing" text
    void  Init(const std::string*, unsigned, unsigned, unsigned);
private:
    std::vector<std::string>  _ResWordsArray; // reserved words
    std::string               _Operators, 
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