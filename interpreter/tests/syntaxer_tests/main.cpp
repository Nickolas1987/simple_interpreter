#include <lexer.h>
#include <interpreter.h>
#include <gtest/gtest.h>
#include <string>
#include <stdio.h>
#include <iostream>
class TestSyntaxer : public ::testing::Test{
protected:
    void SetUp(){
     _interpreter.Init();
    }
    void TearDown(){
    }
    InterpreterNP::CInterpreter _interpreter;
};

TEST_F(TestSyntaxer, test1){
    std::cout << std::endl << "Test arithmetic" << std::endl;
    _interpreter.Run("../resourses/test_prog_2.txt");
    std::cout << std::endl;
}
TEST_F(TestSyntaxer, test2){
    std::cout << std::endl << "Test array and cycle" << std::endl;
    InterpreterNP::CTokenValue arr_val("", E_TOKEN_VALUE_TYPES::tvArray);
    arr_val[0] = InterpreterNP::CTokenValue("2.75", E_TOKEN_VALUE_TYPES::tvDouble);
    arr_val[1] = InterpreterNP::CTokenValue("3.1", E_TOKEN_VALUE_TYPES::tvDouble);
    std::unordered_map<std::string, InterpreterNP::CTokenValue> var = { 
                                                                       {"x", InterpreterNP::CTokenValue("3.15",E_TOKEN_VALUE_TYPES::tvDouble)},
                                                                       { "z", arr_val }
                                                                      };
    std::cout << "input: " << std::endl;
    for (auto iter : var){
     if(iter.second.GetType() != tvArray){
       std::cout << iter.first << " " << iter.second.asString() << std::endl;
     }
     else{
       size_t count = 0;
       while(count < iter.second.GetSize()){
         std::cout << iter.first << " " << iter.second[count].asString() << std::endl;
         ++count;
       }
     }
    }
    _interpreter.Init(var);
    std::cout << std::endl;
    _interpreter.Run("../resourses/test_prog_3.txt");
    std::cout << std::endl;
    std::cout << "ouput: " << std::endl;
    auto out = _interpreter.GetVariables();
    for (auto iter : out){
     if(iter.second.GetType() != tvArray){
       std::cout << iter.first << " " << iter.second.asString() << std::endl;
     }
     else{
       size_t count = 0;
       while(count < iter.second.GetSize()){
         std::cout << iter.first << " " << iter.second[count].asString() << std::endl;
         ++count;
       }
     }
    }
}
TEST_F(TestSyntaxer, test3){
    std::cout << std::endl << "Goto test" << std::endl;
    _interpreter.Init();
    _interpreter.Run("../resourses/test_prog_4.txt");
    std::cout << std::endl;
}


