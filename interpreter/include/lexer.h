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
    std::unordered_set<std::string>  _res_words_array; // reserved words
    std::unordered_set<std::string>  _func_array;
    std::unordered_set<std::string>               _operators, 
                                                   _deviders;
    TokensArray               _tokens_buffer; // tokens after last "lexing"
    unsigned                  _offSet; // current token position
    CToken  SkanToken(const std::string&);
    void  DefineTokenType(CToken&)const;
    CToken SkanStringConstant(const std::string& text);
    void SkipComment(const std::string& text);
    void SkipSpacing(const std::string& text);
};
}
