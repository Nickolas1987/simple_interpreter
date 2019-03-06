#pragma once

#include <string>
#include <token_types.h>
namespace InterpreterNP {
class CLexer;

class CToken {
        friend class CLexer;
    public:
        E_TOKEN_TYPES  Type(void)const { return _type; }
        std::string    Text(void)const { return _text; }

        CToken(void);
        CToken(const CToken&);
        CToken(E_TOKEN_TYPES, const std::string&);

        CToken& operator = (const CToken& other);
        bool operator == (const CToken& other)const;
    private:
        E_TOKEN_TYPES  _type;
        std::string    _text;
};
}
