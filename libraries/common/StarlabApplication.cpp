#include "StarlabApplication.h"
#include "interfaces/FilterPlugin.h"
#include "RichParameterSet.h"

StarlabApplication::StarlabApplication(){
    /// Instantiate resources
    _settings      = new StarlabSettings(this);
    _pluginManager = new PluginManager(_settings);
    _document      = new Document();
    
    /// Register all plugins access functionality
    foreach(StarlabPlugin* plugin, pluginManager()->plugins())
        plugin->_application = this;
}

StarlabApplication::~StarlabApplication(){
    delete _pluginManager;
    delete _document;
    delete _settings;
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
    iIO->save(model,path);            
    
    return true;
}

bool StarlabApplication::loadModel(QString path, InputOutputPlugin* plugin){
    // qDebug("StarlabApplication::loadModel(\"%s\")", qPrintable(path));    
    QFileInfo fileInfo(path);
    QString extension = fileInfo.suffix().toLower();
    QString basename = fileInfo.completeBaseName();
    
    if(plugin==NULL){
        plugin = pluginManager()->modelExtensionToPlugin[extension];
        if(plugin==NULL) return false;
    }
    
    /// Checks a suitable plugin exists
    InputOutputPlugin* iIO = pluginManager()->modelExtensionToPlugin[extension];
    if(iIO == NULL) throw StarlabException("File '%s' has not been opened becase format '%s' not supported", qPrintable(basename), qPrintable(extension));
    
    /// Checks file existence
    iIO->checkReadable(path);
    
    /// Calls the plugin for open operation
    document()->pushBusy();
        Model* newModel = iIO->open(path);
        if(newModel==NULL) throw StarlabException("Attempted to create a NULL model");
        /// Update the bounding box (for rendering reasons)
        /// @todo Would it be possible to Qt-Connect iIO->open to updateBoundingBox()?
        newModel->updateBoundingBox();
        document()->addModel( newModel ); 
    document()->popBusy();

    document()->emit_resetViewport();
    
    return true;
}
bool StarlabApplication::loadProject(QString path, ProjectInputOutputPlugin* plugin){   
    // qDebug("StarlabApplication::loadProject(\"%s\")", qPrintable(path));
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
    
    /// @todo catch exceptions in iIO->open and return false on any caught exception
    return true;
}

QList<FilterPlugin*> StarlabApplication::applicableFilters(){
    return pluginManager()->filterPlugins.values();
}

QList<FilterPlugin *> StarlabApplication::applicableFilters(Model *model){
    QList<FilterPlugin*> retval;    
    foreach(FilterPlugin* plugin, pluginManager()->filterPlugins)
        if(plugin->isApplicable(model))
            retval.append(plugin);
    return retval;
}

void StarlabApplication::load(QString path){
    // qDebug("StarlabApplication::load(\"%s\")", qPrintable(path));
    bool retstatus = false;
    if(!retstatus) retstatus = loadModel(path,NULL);
    if(!retstatus) retstatus = loadProject(path,NULL);
    
    /// Nothing was able to open
    if(!retstatus)
        throw StarlabException("Starlab does not know how to open file: " + path);
}

void StarlabApplication::executeFilter(Model* model, QString filterName){
    DEB qDebug() << "StarlabApplication::executeFilter()";
    FilterPlugin* filter = pluginManager()->filterPlugins.value(filterName,NULL);
    if(filter==NULL) throw StarlabException("Filter '%s' does not exist", qPrintable(filterName));
    if(!filter->isApplicable(model)) throw StarlabException("Filter not applicable");
    
    /// Filter is applied on the *selected* model
    document()->setSelectedModel(model);
    RichParameterSet* pars = new RichParameterSet();
    filter->initParameters(pars);
    filter->applyFilter(pars);
    pars->destructor();
}

QString StarlabApplication::starlabDirectory(){
    QDir baseDir(QApplication::applicationDirPath());
    if( OSQuery::isMac() ){            
        baseDir.cdUp();
        baseDir.cdUp();
        return baseDir.absolutePath();
    }
    if( OSQuery::isWin() )
        return QCoreApplication::applicationDirPath();
    if( OSQuery::isLinux() )
        throw StarlabException("TODO: FIX THE INI LOAD PATH!!!");
    throw StarlabException("TODO: FIX THE INI LOAD PATH!!!");
}

