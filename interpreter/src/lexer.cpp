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
 
    for(unsigned i=0;i<_tokens_buffer.size();++i)
        os<<types[_tokens_buffer[i].Type()]<<
            ": '"<<_tokens_buffer[i].Text()<<"'"<<endl;
}
bool CLexer::Lex(const string& text){
    _offSet = 0;
    _tokens_buffer.clear();
 
    if (text.empty())
        throw std::runtime_error("CLexer::Lex tryng to lex empty text");
 
    unsigned prev_offset = -1;//previous pos
    CToken   token;// current token
    //scan tokens
    do{
        if(prev_offset==_offSet)
            throw std::runtime_error("CLexer::Lex error. Possibly end missed");
 
        prev_offset = _offSet;
        token       = SkanToken(text);

        if(token==ErrorToken)
            return false;
 
        _tokens_buffer.push_back(token);
         if(token.Type() == ttFinish)
            break;
    }while(_offSet < text.length());

    if(_tokens_buffer.back().Type() != ttFinish){
     CToken fin_tok(ttFinish,"");
     _tokens_buffer.push_back(fin_tok);
    }
    return true;
}
const TokensArray& CLexer::GetTokens(void) const { 
    return _tokens_buffer; 
}
CToken CLexer::SkanToken(const string& text){
    CToken ret;
    //separate symbols
    //static const string delim = _Deviders + _Operators;
 
    if(text[_offSet] == '\0'){
        return CToken(ttFinish,"");
    }
    if(text[_offSet] == '"')
        return SkanStringConstant(text);

    if(text[_offSet] == '%')
        SkipComment(text);

    SkipSpacing(text);
    //pos of separ
    unsigned delimpos = text.npos, tmp_pos = text.npos, delimsize = 1;//text.find_first_of(delim, _OffSet);
    
    for (auto iter : _deviders) {
        tmp_pos = text.find(iter, _offSet);
        if (tmp_pos != text.npos) {
            if ((delimpos > tmp_pos) || ((delimpos == tmp_pos) && (delimsize < iter.size()))) {
                delimpos = tmp_pos;
                delimsize = iter.size();
            }
        }
    }
    for (auto iter : _operators) {
        tmp_pos = text.find(iter, _offSet);
        if (tmp_pos != text.npos) {
            if ((delimpos > tmp_pos) || ((delimpos == tmp_pos) && (delimsize < iter.size()))) {
                delimpos = tmp_pos;
                delimsize = iter.size();
            }
        }
    }

    if(_offSet >= text.size())
        return CToken(ttFinish,"");      
    // If this is last lexem
    if(delimpos == text.npos){
        ret._text = text.substr(_offSet);
    }
    else{
        ret._text = text.substr(_offSet, (delimpos == _offSet) ? delimsize : (delimpos - _offSet));//text.substr(_OffSet,max(delimpos - _OffSet,unsigned(delimsize)));
        _offSet = (delimpos == _offSet) ? (_offSet + delimsize) : (delimpos);//max(delimpos,_OffSet+delimsize);
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
        _res_words_array.insert(data[i]);
 
    const string* fend = rwend + fnc;
    for (unsigned i = 0; i < fnc; ++i)
        _func_array.insert(rwend[i]);

    const string* opend = fend + opc;
    for(unsigned i=0;i < opc; ++i)
        _operators.insert(fend[i]);
 
    for(unsigned i=0;i < dvc; ++i)
        _deviders.insert(opend[i]);
}
void CLexer::DefineTokenType(CToken& token)const{
    if(_deviders.find(token._text) != _deviders.end()){
        token._type = ttDevider;
    }
    else if(_operators.find(token._text) != _operators.end()){
        token._type = ttOperator;
    }
    else if(_res_words_array.find(token._text) != _res_words_array.end()){
        token._type = ttResWord;
    }
    else if (_func_array.find(token._text) != _func_array.end()) {
        token._type = ttFunction;
    }
    else{
        unsigned i;
        for(i = 0; i < token._text.length(); ++i)
            if(!isdigit(token._text[i]) && token._text[i] != '.')
                break;
 
        if(i==token._text.length()){
            if(token._text.find('.') != std::string::npos)
             token._type = ttDoubleConstant;
            else
             token._type = ttIntConstant;
        }
        else
            token._type = ttVariable;
    }
}
CToken CLexer::SkanStringConstant(const string& text){
    const unsigned pos = text.find_first_of('"',++_offSet); //find ended "
 
    if(pos == text.npos)
        throw runtime_error("CLexer::SkanStringConstant: '' expected");

    const unsigned begin = _offSet;
    _offSet = pos+1;
 
    return CToken(ttStrConstant, string(text, begin, pos - begin));
}
void CLexer::SkipSpacing(const string& text){
    if(!text[_offSet] || !strchr(" \t\r\n", text[_offSet]))
        return;
    while(_offSet < text.size() && strchr(" \t\r\n", text[_offSet]))++_offSet;
    --_offSet;
}
void CLexer::SkipComment(const string& text){
    while(text[_offSet]=='%'){
     _offSet = text.find_first_of("\n", _offSet);
     if(_offSet == text.npos)
       break;
     ++_offSet;
    }
}
}
