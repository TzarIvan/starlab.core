#pragma once 
#include <QObject>

#ifdef WIN32
    #define VSPRINTF vsprintf_s
#else
    #define VSPRINTF vsprintf
#endif

/// @todo re-think logging system or simply eliminate it
class Logger : public QObject{
    Q_OBJECT
    
    /// Maybe use flags for multiple usage?
    // enum LOGTYPE{STATUSBAR, TERMINAL};
    
public:
    // void Log(int Level, const char * f, ... ) ;
    
    void Log(const char *format, ...){
        char buffer[256];
        va_list args;
        va_start (args, format);
        VSPRINTF(buffer,format, args);
        va_end (args);
        QString todo(buffer);
        emit showmessage( QString(buffer) );
    }

signals:
    void showmessage(QString message);
};
