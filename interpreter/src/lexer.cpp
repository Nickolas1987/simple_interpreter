#include "lexer.h"
#include "log.h"
#include <ctype.h>
#include <algorithm>
#include <string.h>
using namespace std;

namespace InterpreterNP{
CToken ErrorToken(ttUnknown, "ErrorToken");

void CLexer::SaveTokens(ostream& os)const{
    if(os.bad())
        MACRO_ERROR_RET_VOID("CLexer::SaveTokens error");
 
    string types[] = {"ttResWord",     "ttOperator", "ttStrConstant",
                      "ttIntConstant", "ttDoubleConstant", "ttVariable", "ttFunction",
                      "ttDevider",     "ttUnknown", "ttFinish"};
 
    for(unsigned i=0;i<_TokensBuffer.size();++i)
        os<<types[_TokensBuffer[i].Type()]<<
            ": '"<<_TokensBuffer[i].Text()<<"'"<<endl;
}
bool CLexer::Lex(const string& text){
    _OffSet = 0;
    _TokensBuffer.clear();
 
    if (text.empty())
        MACRO_ERROR_RET("CLexer::Lex tryng to lex empty text", false);
 
    unsigned prev_offset=-1;//previous pos
    CToken   token;// current token
    //scan tokens
    do{
        if(prev_offset==_OffSet)
            MACRO_ERROR_RET("CLexer::Lex error. Possibly end missed",false);
 
        prev_offset = _OffSet;
        token       = SkanToken(text);

        if(token==ErrorToken)
            return false;
 
        _TokensBuffer.push_back(token);
         if(token.Type() == ttFinish)
            break;
    }while(_OffSet < text.length());

    if(_TokensBuffer.back().Type() != ttFinish){
     CToken fin_tok(ttFinish,"");
     _TokensBuffer.push_back(fin_tok);
    }

    return true;
}
CToken CLexer::SkanToken(const string& text){
    CToken ret;
    //separate symbols
    static const string delim = _Deviders + _Operators;
    //pos of separ
    const unsigned delimpos = text.find_first_of(delim, _OffSet);
 
    if(text[_OffSet] == '\0'){
        return CToken(ttFinish,"");
    }
    if(text[_OffSet] == '"')
        return SkanStringConstant(text);

    if(text[_OffSet] == '%')
        SkipComment(text);

    SkipSpacing(text);
   // If this is last lexem
    if(delimpos == text.npos){
        ret._Text = text.substr(_OffSet);
    }
    else{
        ret._Text = text.substr(_OffSet,max(delimpos-_OffSet,unsigned(1)));
        _OffSet = max(delimpos,_OffSet+1);
    }

    DefineTokenType(ret);
    return ret;
}
void  CLexer::Init(//array of strings data
                   const string* data,
                   unsigned rwc,  // count of reserved words
                   unsigned opc,  //count of operators
                   unsigned dvc)  //count of deviders
{
    _ResWordsArray.resize(rwc);
    const string* rwend = data + rwc;
    copy(data,rwend,_ResWordsArray.begin());
 
    const string* opend = rwend + opc;
    for(unsigned i=0;i < opc; ++i)
        _Operators += rwend[i];
 
    for(unsigned i=0;i < dvc; ++i)
        _Deviders += opend[i];
}
void CLexer::DefineTokenType(CToken& token)const{
    if(_Deviders.find(token.Text()) != _Deviders.npos){
        token._Type = ttDevider;
    }
    else if(_Operators.find(token.Text()) != _Operators.npos){
        token._Type = ttOperator;
    }
    else if(find(_ResWordsArray.begin(),_ResWordsArray.end(),token.Text()) != _ResWordsArray.end()){
        token._Type = ttResWord;
    }
    else{
        unsigned i;
        for(i = 0; i < token._Text.length(); ++i)
            if(!isdigit(token.Text()[i]) && token.Text()[i] != '.')
                break;
 
        if(i==token.Text().length()){
            if(token.Text().find('.') != std::string::npos)
             token._Type = ttDoubleConstant;
            else
             token._Type = ttIntConstant;
        }
        else
            token._Type = ttVariable;
    }
}
CToken CLexer::SkanStringConstant(const string& text){
    const unsigned pos = text.find_first_of('"',++_OffSet); //find ended "
 
    if(pos==text.npos)
        MACRO_ERROR_RET("CLexer::SkanStringConstant: '' expected",ErrorToken);

    const unsigned begin = _OffSet;
    _OffSet = pos+1;
 
    return CToken(ttStrConstant, string(text, begin, pos-begin));
}
void CLexer::SkipSpacing(const string& text){
    if(!text[_OffSet] || !strchr(" \t\r\n", text[_OffSet]))
        return;
    _OffSet = text.find_first_not_of(" \t\r\n", _OffSet) - 1;
}
void CLexer::SkipComment(const string& text){
    while(text[_OffSet]=='%')
        _OffSet = text.find_first_of("\n", _OffSet) + 1;
}
}
