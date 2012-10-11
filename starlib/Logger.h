#pragma once 
#include <QObject>
#include "starlib_global.h"

#ifdef WIN32
    #define VSPRINTF vsprintf_s
#else
    #define VSPRINTF vsprintf
#endif

/// @todo re-think logging system or simply eliminate it
class STARLIB_EXPORT Logger : public QObject{
    Q_OBJECT
    
    /// Maybe use flags for multiple usage?
    // enum LOGTYPE{STATUSBAR, TERMINAL};
    
public:
    // void Log(int Level, const char * f, ... ) ;
    
    void Log(const char *format, ...){
        const size_t buffer_length=256;
        char buffer[buffer_length];
        va_list args;
        va_start (args, format);
        vsnprintf(buffer,buffer_length,format, args);
        va_end (args);
        QString todo(buffer);
        emit showmessage( QString(buffer) );
    }

signals:
    void showmessage(QString message);
};
