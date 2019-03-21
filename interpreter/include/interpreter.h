#pragma once
#include <lexer.h>
#include <syntaxer.h>
#include <token_value.h>
#include <functional>
#include <unordered_map>
#include <string>
namespace InterpreterNP{
class CInterpreter{
public:
    CInterpreter();
    virtual bool Init(const std::unordered_map<std::string, CTokenValue>& variables = std::unordered_map<std::string, CTokenValue>());
    virtual bool Run(const std::string& file_name);
    virtual std::unordered_map<std::string, CTokenValue> GetVariables() const;
protected:
    CInterpreter(const CInterpreter&) = delete;
    CInterpreter& operator=(const CInterpreter&) = delete;
    void InputFunc();
    void PrintFunc();
    void IfFunc();
    void WhileFunc();
    void ExitFunc();
    void GotoFunc();
    CLexer _lexer;
    CSyntaxer _syntaxer;
    std::unordered_map<std::string, CTokenValue> _variables;
};
}
