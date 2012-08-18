#pragma once 

/// @todo re-think logging system or simply eliminate it
class Logger{
private:
    // GLLogStream *log;
public:
    Logger(){}
    virtual ~Logger() {}
public:
    /// Standard stuff that usually should not be redefined.
    // void setLog(GLLogStream *log) { this->log = log ; }
    void Log(const char * f, ... );
    void Log(int Level, const char * f, ... ) ;
};
