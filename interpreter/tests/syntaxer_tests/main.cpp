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
    _interpreter.Run("../resourses/test_prog_2.txt");
}


