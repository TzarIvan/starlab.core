#include <QDebug>
#include <QColor>
#include <QGLFormat> /// @todo why in main.cpp???
#include "StarlabMainWindow.h"
#include "CmdLineParser.h"
#include "StarlabApplication.h"
#include "StarlabSettings.h"
#include "FileOpenEater.h"
#include "interfaces/FilterPlugin.h"

int main_(int argc, char *argv[]) { 

    QCoreApplication::setOrganizationName("Free Software Foundation");
    QCoreApplication::setApplicationName("starlab");
    QCoreApplication::setApplicationVersion("1.0.1");

    /// Create a starlab application
    StarlabApplication application(argc,argv);
    /// Parse command line into StarlabSettings
    CmdLineParser(argc,argv,application.settings());     
            
    //qDebug() << qPrintable(settings);
    
    // Anti-aliasing when using QGLWidget or subclasses
    QGLFormat glf = QGLFormat::defaultFormat();
    glf.setSamples(8);
    QGLFormat::setDefaultFormat(glf);

#if 0
    /// Lists filters if requested
    if(application.document()->getString("action")=="list-filters"){
        qDebug() << "Available filters: ";
        foreach(FilterPlugin* p, application.pluginManager.filterPlugins.values())
            qDebug() << "  " << qPrintable( p->name() );
        exit(0);        
    }
#endif
    
    /// Create window (@internal *new* is important)
    StarlabMainWindow* mainWindow = new StarlabMainWindow(&application);        
    
    /// @todo Automatically load layer menu if I opened more than one model    
    // if(mainWindow->document()->models.size()>=2)
    //    emit mainWindow->show_layer_dialog(true);    

    /// Loads the input models
    /// This event filter is installed to intercept the open events sent directly by the Operative System.
    {
        /// Install OS file I/O handler
        FileOpenEater* eater = new FileOpenEater(mainWindow);
        application.installEventFilter(eater);
        application.processEvents();
        
        /// Pass input files to handler
        QStringList inputs = application.settings()->getStringList("inputs");
        foreach(QString file, inputs)
            QApplication::sendEvent(&application, new QFileOpenEvent(file));
    } 
    
    /// Shows a filter GUI if it has one specified in its settings
#if 0
    QString filterName = document()->getString("filter");
    if(!filterName.isEmpty()) mainWindow->triggerFilterByName(filterName);
#endif
    
    /// Starts the event loop
    return application.exec();    
}

int main(int argc, char *argv[]){
    try 
    {
        return main_(argc, argv);        
    } 
    STARLAB_CATCH_BLOCK
}
