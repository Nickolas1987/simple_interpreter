#include <lexer.h>
#include <ctype.h>
#include <algorithm>
#include <string.h>
#include <stdexcept>
using namespace std;

namespace InterpreterNP{
CToken ErrorToken(ttUnknown, "ErrorToken");

void CLexer::SaveTokens(ostream& os)const{
    if(os.bad())
        throw runtime_error("CLexer::SaveTokens error");
 
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
        throw std::runtime_error("CLexer::Lex tryng to lex empty text");
 
    unsigned prev_offset = -1;//previous pos
    CToken   token;// current token
    //scan tokens
    do{
        if(prev_offset==_OffSet)
            throw std::runtime_error("CLexer::Lex error. Possibly end missed");
 
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
const TokensArray& CLexer::GetTokens(void) const { 
    return _TokensBuffer; 
}
CToken CLexer::SkanToken(const string& text){
    CToken ret;
    //separate symbols
    //static const string delim = _Deviders + _Operators;
 
    if(text[_OffSet] == '\0'){
        return CToken(ttFinish,"");
    }
    if(text[_OffSet] == '"')
        return SkanStringConstant(text);

    if(text[_OffSet] == '%')
        SkipComment(text);

    SkipSpacing(text);
    //pos of separ
    unsigned delimpos = text.npos, tmp_pos = text.npos, delimsize = 1;//text.find_first_of(delim, _OffSet);
    
    for (auto iter : _Deviders) {
        tmp_pos = text.find(iter, _OffSet);
        if (tmp_pos != text.npos) {
            if ((delimpos > tmp_pos) || ((delimpos == tmp_pos) && (delimsize < iter.size()))) {
                delimpos = tmp_pos;
                delimsize = iter.size();
            }
        }
    }
    for (auto iter : _Operators) {
        tmp_pos = text.find(iter, _OffSet);
        if (tmp_pos != text.npos) {
            if ((delimpos > tmp_pos) || ((delimpos == tmp_pos) && (delimsize < iter.size()))) {
                delimpos = tmp_pos;
                delimsize = iter.size();
            }
        }
    }

    if(_OffSet >= text.size())
        return CToken(ttFinish,"");      
    // If this is last lexem
    if(delimpos == text.npos){
        ret._Text = text.substr(_OffSet);
    }
    else{
        ret._Text = text.substr(_OffSet, (delimpos == _OffSet) ? delimsize : (delimpos - _OffSet));//text.substr(_OffSet,max(delimpos - _OffSet,unsigned(delimsize)));
        _OffSet = (delimpos == _OffSet) ? (_OffSet + delimsize) : (delimpos);//max(delimpos,_OffSet+delimsize);
    }
    DefineTokenType(ret);
    return ret;
}
void  CLexer::Init(//array of strings data
                   const string* data,
                   unsigned rwc,  // count of reserved words
                   unsigned fnc,  // count of func
                   unsigned opc,  //count of operators
                   unsigned dvc)  //count of deviders
{
    const string* rwend = data + rwc;
    for (unsigned i = 0; i < rwc; ++i)
        _ResWordsArray.insert(data[i]);
 
    const string* fend = rwend + fnc;
    for (unsigned i = 0; i < fnc; ++i)
        _FuncArray.insert(rwend[i]);

    const string* opend = fend + opc;
    for(unsigned i=0;i < opc; ++i)
        _Operators.insert(fend[i]);
 
    for(unsigned i=0;i < dvc; ++i)
        _Deviders.insert(opend[i]);
}
void CLexer::DefineTokenType(CToken& token)const{
    if(_Deviders.find(token.Text()) != _Deviders.end()){
        token._Type = ttDevider;
    }
    else if(_Operators.find(token.Text()) != _Operators.end()){
        token._Type = ttOperator;
    }
    else if(_ResWordsArray.find(token.Text()) != _ResWordsArray.end()){
        token._Type = ttResWord;
    }
    else if (_FuncArray.find(token.Text()) != _FuncArray.end()) {
        token._Type = ttFunction;
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
 
    if(pos == text.npos)
        throw runtime_error("CLexer::SkanStringConstant: '' expected");

    const unsigned begin = _OffSet;
    _OffSet = pos+1;
 
    return CToken(ttStrConstant, string(text, begin, pos - begin));
}
void CLexer::SkipSpacing(const string& text){
    if(!text[_OffSet] || !strchr(" \t\r\n", text[_OffSet]))
        return;
    while(_OffSet < text.size() && strchr(" \t\r\n", text[_OffSet]))++_OffSet;
    --_OffSet;
}
void CLexer::SkipComment(const string& text){
    while(text[_OffSet]=='%'){
     _OffSet = text.find_first_of("\n", _OffSet);
     if(_OffSet == text.npos)
       break;
     ++_OffSet;
    }
}
}
