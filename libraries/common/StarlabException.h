#pragma once
#include <cstdio>
#include <stdarg.h>
#include <QString>

#include "OSQuery.h"
#include <QMessageBox>

class StarlabException : public std::exception{
protected:  
    QString errmsg;
    
public:
    void init(){
        errmsg = "error not specified";
    }
    StarlabException(){ init(); }
    StarlabException(const QString& text){ 
        init();
        errmsg = text; 
    }
       
    /// This throws an exeption with a format similar to a printf
    /// Make sure you are passing actual char* or it will terminate badly!!
    StarlabException(const char *format, ...){
        char buffer[256];
        va_list args;
        va_start (args, format);

#ifdef WIN32
        vsprintf_s (buffer,format, args);
#else
        vsprintf (buffer,format, args);
#endif

        va_end (args);
        errmsg = QString(buffer);
    }
    
    ~StarlabException() throw() {}

    /// For std::exception
    virtual inline const char* what() const throw() {
        return qPrintable(errmsg);
    }
     
    /// Used in the title of the messagebox
    virtual QString type(){ return "Starlab Exception"; }
    virtual QString message(){ return errmsg; }
};

class StarlabNotImplementedException : public StarlabException{
public:
    StarlabNotImplementedException(const QString& text) : StarlabException("Method not implemented: " + text){}    
};


/// Default try/catch blocks, use them as:
/// 
/// try
/// {
///    ... potential exc-throwing code ...
/// }
/// STARLAB_CATCH_BLOCK
#define STARLAB_CATCH_BLOCK \
    catch(StarlabException& e){ \
        qApp->restoreOverrideCursor(); \
        if(OSQuery::isMac()) QMessageBox::warning(NULL, e.type(), e.type().toUpper() + ":\n\n" + e.message()); \
        else QMessageBox::warning(NULL, e.type(), e.message()); \
        qWarning() << "[StarlabException]: " << e.message(); \
    } catch(std::exception& e){ \
        qWarning() << "[std::exception]" << e.what(); \
        exit(-1); \
    } catch (...) { \
        qDebug() << "Bad exception!!"; \
        exit(-1); \
    } 
