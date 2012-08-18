#pragma once
#include "StarlabMainWindow.h"
class FileOpenEater : public QObject{
  Q_OBJECT
private:
    StarlabMainWindow* const mainWindow;
public:
    FileOpenEater(StarlabMainWindow* mainWindow) : mainWindow(mainWindow){}       
protected:
    bool eventFilter(QObject *obj, QEvent *event){
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *fileEvent = static_cast<QFileOpenEvent*>(event);
            mainWindow->application()->load( fileEvent->file() );
            return true;
        } else {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
    }
};
