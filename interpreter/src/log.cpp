#include "log.h"
namespace InterpreterNP{
void  CLogFile::WriteMessage(const char* mes){
    _log_file << mes;
}
void  CLogFile::WriteWarning(const char* mes){
    _log_file<<mes;
}
void  CLogFile::WriteError(const char* mes){
   _log_file<<mes;
}
CLogFile::CLogFile(const char* name){
    _log_file.open(name);
    if(!_log_file.is_open())
        throw std::runtime_error("Can't open log");
}
CLogFile::~CLogFile(void){
    _log_file.close();
}
}
InterpreterNP::CLogFile ESLLog("ESLlog.txt");

