#pragma once

#include <string>
#include <token_types.h>
namespace InterpreterNP {
class CLexer;

class CToken {
        friend class CLexer;
    public:
        E_TOKEN_TYPES  Type(void)const { return _Type; }
        std::string    Text(void)const { return _Text; }

        CToken(void);
        CToken(const CToken&);
        CToken(E_TOKEN_TYPES, const std::string&);

        CToken& operator = (const CToken& other);
        bool operator == (const CToken& other)const;
    private:
        E_TOKEN_TYPES  _Type;
        std::string    _Text;
};
}
