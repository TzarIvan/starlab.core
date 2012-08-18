#include "StarlabApplication.h"
#include "interfaces/FilterPlugin.h"

StarlabApplication::StarlabApplication(int& argc, char* argv[])
    : QApplication(argc,argv){
    QLocale::setDefault(QLocale::C); // Use "C"urrent locale
    
    _settings = new StarlabSettings();
    _pluginManager = new PluginManager(_settings);
    _document = new Document();
    
    /// Register all plugins access functionality
    foreach(StarlabPlugin* plugin, pluginManager()->plugins())
        plugin->_application = this;
}

StarlabApplication::~StarlabApplication(){
    delete _pluginManager;
    delete _document;
    delete _settings;
}

bool StarlabApplication::notify( QObject * rec, QEvent * ev ){
    try{
        return QApplication::notify(rec,ev);
    } catch (StarlabException& e){        
        QString title = "Unmanaged StarLab Exception";
        QString message = e.what();
        int retval = QMessageBox::critical(NULL, title, message, QMessageBox::Abort|QMessageBox::Ignore);
        if( retval == QMessageBox::Abort ){
            qCritical() << "Terminated because aborted exception";
            // this->closeAllWindows();
            this->exit(-1);
        }
        /// Restore pointer if it was changed
        qApp->restoreOverrideCursor();
    } catch (...) {
        qDebug() << "BAD EXCEPTION..";
    }
    // the event will be propagated to the receiver's parent and 
    // so on up to the top-level object if the receiver is not 
    // interested in the event (i.e., it returns false).
    return false;
}

bool StarlabApplication::saveModel(Model* model, QString path){
    if(path.isEmpty()) path=model->path;
    DEB qDebug() << "Saving model: " << model->name << " to file: " << path;
    QFileInfo fileInfo(path);
    
    /// Extract or set default extension
    QString extension = fileInfo.suffix().toLower();
    DEB qDebug() << "requested extension: " << extension;
    if( extension.isEmpty() ) extension = "off";        
    
    /// Checks a suitable plugin exists
    InputOutputPlugin* iIO = pluginManager()->modelExtensionToPlugin[extension];
    if( !iIO ) throw StarlabException("Cannot find plugin suitable for the provided extension %s", qPrintable(extension));
    
    qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
        iIO->save(path, model);            
    qApp->restoreOverrideCursor();
    
    return true;
}

bool StarlabApplication::loadModel(QString path, InputOutputPlugin* plugin){
    qDebug("StarlabApplication::loadModel(\"%s\")", qPrintable(path));
    
    QFileInfo fileInfo(path);
    QString extension = fileInfo.suffix().toLower();
    QString basename = fileInfo.completeBaseName();
    
    if(plugin==NULL){
        plugin = pluginManager()->modelExtensionToPlugin[extension];
        if(plugin==NULL) return false;
    }
    
    /// Checks a suitable plugin exists
    InputOutputPlugin* iIO = pluginManager()->modelExtensionToPlugin[extension];
    
    /// Checks file existence
    if(iIO == NULL)            throw StarlabException("File '%s' has not been opened, format %s not supported", qPrintable(basename), qPrintable(extension));
    if(!fileInfo.exists())     throw StarlabException("File '%s' does not exist", qPrintable(path));
    if(!fileInfo.isReadable()) throw StarlabException("File '%s' is not readable", qPrintable(path));
    
    /// Calls the plugin for open operation
    document()->pushBusy();
        Model* newModel = iIO->open(path);
        if(newModel==NULL) throw StarlabException("Attempted to create a NULL model");
        /// Update the bounding box (for rendering reasons)
        /// @todo Would it be possible to Qt-Connect iIO->open to updateBoundingBox()?
        newModel->updateBoundingBox();
        document()->addModel( newModel ); 
    document()->popBusy();
    
    if(document()->models().size()==1)
        document()->emit_resetViewport();
    
    return true;
}
bool StarlabApplication::loadProject(QString path, ProjectInputOutputPlugin* plugin){
    qDebug("StarlabApplication::loadProject(\"%s\")", qPrintable(path));
    
    QFileInfo fileInfo(path);
    QString extension = fileInfo.suffix().toLower();
    QString basename = fileInfo.completeBaseName();    

    if(plugin==NULL){
        plugin = pluginManager()->projectExtensionToPlugin[extension];
        if(plugin==NULL) 
            return false;
    }
    
    /// Checks a suitable plugin exists   
    ProjectInputOutputPlugin* iIO = pluginManager()->projectExtensionToPlugin[extension];
    
    /// Checks file existence
    if(iIO == NULL)            throw StarlabException("Project file '%s' has not been opened, format %s not supported", qPrintable(basename), qPrintable(extension));
    if(!fileInfo.exists())     throw StarlabException("Project file '%s' does not exist", qPrintable(path));
    if(!fileInfo.isReadable()) throw StarlabException("Project file '%s' is not readable", qPrintable(path));
    
    /// Clear the doc and use plugin to fill in
    document()->pushBusy();
        document()->clear();
        iIO->open(path, *this);
    document()->popBusy();
    
    /// Refresh visualization
    document()->emit_resetViewport();
    document()->emit_hasChanged();
    
    /// @todo catch exceptions in iIO->open and return false on any caught exception
    return true;
}

void StarlabApplication::load(QString path){
    try{
        // qDebug("StarlabApplication::load(\"%s\")", qPrintable(path));
        bool retstatus = false;
        if(!retstatus) retstatus = loadModel(path,NULL);
        if(!retstatus) retstatus = loadProject(path,NULL);
        
        /// Nothing was able to open
        if(!retstatus)
            throw StarlabException("Starlab does not know how to open file: \"%s\"",qPrintable(path));
    } STARLAB_CATCH_BLOCK
}

void StarlabApplication::executeFilter(QString filterName){
    DEB qDebug() << "StarlabApplication::executeFilter()";
    FilterPlugin* filter = pluginManager()->filterPlugins[ filterName ];
    foreach(Model* model, document()->models()){
        document()->setSelectedModel(model);
        RichParameterSet* pars = new RichParameterSet();
        /// @todo have "dummy" drawArea here which throws exceptions on misuse
        filter->initParameters(pars);
        filter->applyFilter(pars);
        pars->destructor();
    }
}

