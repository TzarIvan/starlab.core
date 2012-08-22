#include <QDebug>
#include <QColor>
#include <QGLFormat> /// @todo why in main.cpp???
#include "StarlabMainWindow.h"
#include "GUIApplication.h"
#include "StarlabApplication.h"
#include "StarlabSettings.h"
#include "FileOpenEater.h"
#include "interfaces/FilterPlugin.h"

int main(int argc, char *argv[]) { 
    try{    
        /// Create QT GUI app
        GUIApplication app(argc,argv);
        
        /// Create a starlab application
        StarlabApplication application;    
        
        /// Create a new window (@internal *new* is important)
        StarlabMainWindow* mainWindow = new StarlabMainWindow(&application);        
        
        /// Manages I/O requested by Operating system
        FileOpenEater* eater = new FileOpenEater(mainWindow);
        // app.installEventFilter(eater);
        mainWindow->installEventFilter(eater);
        
        /// Open command line input
        for(int i=1; i<argc; i++)
            QApplication::sendEvent(mainWindow, new QFileOpenEvent(argv[i]));
        
        /// Automatically load layer menu if I opened more than one model    
        // if(mainWindow->document()->models.size()>=2)
        //    emit mainWindow->show_layer_dialog(true);    
    
        /// Show the window 
        /// (@internal see QWidget::activateWindow)
        mainWindow->showNormal();
        mainWindow->activateWindow();
        mainWindow->raise();
        
        /// Starts the event loop
        return app.exec();    
    }
    STARLAB_CATCH_BLOCK
}
