#include <token.h>

using namespace std;
namespace InterpreterNP {
    CToken::CToken(void) : _Type(ttUnknown) {};
    CToken::CToken(const CToken& other) : _Type(other._Type), _Text(other._Text) {};
    CToken::CToken(E_TOKEN_TYPES type, const string& text) :
        _Type(type), _Text(text) {};
    CToken& CToken::operator=(const CToken& other) {
        if (&other != this) {
            _Type = other._Type;
            _Text = other._Text;
        }
        return *this;
    }
    bool CToken::operator==(const CToken& other)const {
        return _Type == other._Type && _Text == other._Text;
    }
}
