#pragma once
#include "lexer.h"
#include "syntaxer.h"
#include <functional>
namespace InterpreterNP{
class CInterpreter{
public:
    CInterpreter();
    virtual bool Init();
    virtual bool Run(const std::string& file_name);
protected:
    CInterpreter(const CInterpreter&);
    CInterpreter& operator=(const CInterpreter&);
    void InputFunc();
    void PrintFunc();
    void IfFunc();
    void WhileFunc();
    CLexer _lexer;
    CSyntaxer _syntaxer;
};
}