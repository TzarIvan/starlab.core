#pragma once
#include "StarlabMainWindow.h"
class FileOpenEater : public QObject{
  Q_OBJECT
private:
    StarlabMainWindow* const mainWindow;
public:
    FileOpenEater(StarlabMainWindow* mainWindow) : mainWindow(mainWindow){
        setParent(mainWindow);
    }       
protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
            QString target = fileEvent->file();
            mainWindow->statusBarMessage("Loading file '"+target+"'",0.0);
            mainWindow->application()->load(target);
            mainWindow->statusBarMessage("The file '"+target+"' has been loaded",2.0);
            return true;
        } else {
            /// use standard event processing
            return QObject::eventFilter(obj, event);
        }
    }
};
