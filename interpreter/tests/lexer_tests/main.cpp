#include <lexer.h>
#include <gtest/gtest.h>
#include <string>
#include <stdio.h>
#include <iostream>
class TestLexer : public ::testing::Test{
protected:
    void SetUp(){
      std::string data[]={"if","while","print","input",
               "+", "-", "=", "/", "*", ">", "<", "~", "!", "|", "&",
               " ", ";", "(", ")", ",", "{", "}", "\n", "\t", "\r", "%"};
      _lexer.Init(data,4,11,11);
    }
    void TearDown(){
    }
    InterpreterNP::CLexer _lexer;
};

TEST_F(TestLexer, test1){

    FILE *f = fopen("../resourses/test_prog.txt","rt");
    if(!f){
        FAIL() << "no file for lex";
        return ;
    }
    fseek(f, 0, SEEK_END); long len = ftell(f); fseek(f, 0, SEEK_SET);
    std::string str;
    str.resize(len);
    fread(&str[0],1,len,f);
    fclose(f);

   _lexer.Lex(str);
   _lexer.SaveTokens(std::cout);
}


