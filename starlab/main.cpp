#include <QDebug>
#include <QColor>
#include <QGLFormat> /// @todo why in main.cpp???
#include "StarlabMainWindow.h"
#include "StarlabApplication.h"
#include "StarlabSettings.h"
#include "FileOpenEater.h"
#include "interfaces/FilterPlugin.h"

int main(int argc, char *argv[]) { 
    try{
        QApplication::setOrganizationName("Free Software Foundation");
        QApplication::setApplicationName("starlab");
        QApplication::setApplicationVersion("1.0.1");
    
        /// Create a starlab application
        StarlabApplication application(argc,argv);    
        /// Create window (@internal *new* is important)
        StarlabMainWindow* mainWindow = new StarlabMainWindow(&application);        
        
        /// Manages open requests from OS (i.e. double click a file)
        FileOpenEater* eater = new FileOpenEater(mainWindow);
        application.installEventFilter(eater);
        application.processEvents();
        
        /// Attempts to open varargs (@internal skips program name)
        for(int i=1; i<argc; i++)
            QApplication::sendEvent(&application, new QFileOpenEvent(argv[i]));
        
        /// Automatically load layer menu if I opened more than one model    
        // if(mainWindow->document()->models.size()>=2)
        //    emit mainWindow->show_layer_dialog(true);    
    
        /// Starts the event loop
        return application.exec();    
    }
    STARLAB_CATCH_BLOCK   
}
