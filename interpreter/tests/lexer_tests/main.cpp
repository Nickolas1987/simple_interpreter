#include <lexer.h>
#include <gtest/gtest.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <streambuf>
class TestLexer : public ::testing::Test{
protected:
    void SetUp(){
      std::string data[]={"if","while","print","input","exit",
                   "count",
                   "+", "-", "=", "/", "*", "^", ">", "<", "==", "!=", ">=", "<=", "~", "!", "||", "&&",
                   " ", ";", "(", ")", ",", "{", "}", "[", "]", "\n", "\t", "\r", "%"
                  };
      _lexer.Init(data, 5, 1, 16, 13);
    }
    void TearDown(){
    }
    InterpreterNP::CLexer _lexer;
};

TEST_F(TestLexer, test1){
    std::ifstream _file("../resourses/test_prog_2.txt");
    if(!_file.is_open()){
     FAIL() << "no such file for lexing";
     return;
    }
    std::string _str((std::istreambuf_iterator<char>(_file)),
                 std::istreambuf_iterator<char>());
    _lexer.Lex(_str);

   _lexer.SaveTokens(std::cout);
}


