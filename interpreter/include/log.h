#pragma once
#include <fstream>
namespace InterpreterNP{
class CLogFile{

 public:
    std::ofstream& LogFile(void)           {return _log_file;} 
    //any message
    void  WriteMessage(const char*);
    //write message about warning
    void  WriteWarning(const char*);
    //write error message
    void  WriteError(const char*);

    CLogFile(const char* name);
    
    ~CLogFile(void);

 private:
    std::ofstream _log_file;
};
}
extern InterpreterNP::CLogFile ESLLog;
 
#ifndef _ESLNDEBUG
  #define MACRO_ERROR_RET(err,ret) {ESLLog.WriteError(err);return(ret);}
  #define MACRO_ERROR_RET_VOID(err) {ESLLog.WriteError(err);return;}
#else
  #define MACRO_ERROR_RET(err,ret)  {return(ret);}
  #define MACRO_ERROR_RET_VOID(err) {return;}
#endif
