#include "interpreter.h"
#include "log.h"
using namespace InterpreterNP;
int main(int argc, char* argv[]) {
    try {
        if(argc > 1){
          CInterpreter _interpreter;
          _interpreter.Init();
          _interpreter.Run(argv[1]);
        }
        else
          ESLLog.WriteError("No file for interpretation!");
    }
    catch (std::exception& e) {
        ESLLog.WriteError(e.what());
    }
    return 0;
}
