#include <token.h>

using namespace std;
namespace InterpreterNP {
    CToken::CToken(void) : _type(ttUnknown) {};
    CToken::CToken(const CToken& other) : _type(other._type), _text(other._text) {};
    CToken::CToken(E_TOKEN_TYPES type, const string& text) :
        _type(type), _text(text) {};
    CToken& CToken::operator=(const CToken& other) {
        if (&other != this) {
            _type = other._type;
            _text = other._text;
        }
        return *this;
    }
    bool CToken::operator==(const CToken& other)const {
        return _type == other._type && _text == other._text;
    }
}
