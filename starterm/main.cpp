#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
 
    #if 0
        /// Lists filters if requested
        if(application.document()->getString("action")=="list-filters"){
            qDebug() << "Available filters: ";
            foreach(FilterPlugin* p, application.pluginManager.filterPlugins.values())
                qDebug() << "  " << qPrintable( p->name() );
            exit(0);        
        }
    #endif
        
    /// Shows a filter GUI if it has one specified in its settings
    #if 0
        QString filterName = document()->getString("filter");
        if(!filterName.isEmpty()) mainWindow->triggerFilterByName(filterName);
    #endif
    
    return a.exec();
}
