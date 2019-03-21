#pragma once
namespace InterpreterNP{
enum E_TOKEN_TYPES
{
    ttResWord = 0,
    ttOperator,
    ttStrConstant,
    ttIntConstant,
    ttDoubleConstant,
    ttVariable,    
    ttFunction,
    ttLabel,
    ttDevider,     
    ttUnknown,      
    ttFinish       
};
}
