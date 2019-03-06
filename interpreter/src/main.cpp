#include <interpreter.h>
#include <iostream>
using namespace InterpreterNP;
int main(int argc, char* argv[]) {
    try {
        if(argc > 1){
          CInterpreter _interpreter;
          _interpreter.Init();
          _interpreter.Run(argv[1]);
        }
        else
          std::cout << "No file for interpretation!" << std::endl;
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
