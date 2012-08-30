#include "PluginManager.h"
#include <QObject>
#include <QRegExp>
#include <QtScript/QtScript>

#include "StarlabException.h"
#include "StarlabSettings.h"
#include "interfaces/RenderPlugin.h"
#include "interfaces/DecoratePlugin.h"
#include "interfaces/FilterPlugin.h"
#include "interfaces/GuiPlugin.h"
#include "interfaces/ModePlugin.h"

#if defined(Q_OS_WIN)
    QString LIB_EXTENSION_FILTER("*.dll");
#elif defined(Q_OS_MAC)
    QString LIB_EXTENSION_FILTER("*.dylib");
#elif defined(Q_OS_LINUX)
    QString LIB_EXTENSION_FILTER("*.so");
#endif

/// Input: a string like "[SurfaceMesh] Object File Format (*.off,*.off2)"
/// Returns a lowercase stringlist: {"off","off2"}
QStringList extractExtensions(QString iopluginname){
    QStringList retval;
    
    // qDebug() << "Parsing string: " << iopluginname;
    int i_open  = iopluginname.indexOf("(");
    int i_close = iopluginname.indexOf(")");
    
    // Chunk of string between parenthesis, then split by comma
    iopluginname = iopluginname.mid(i_open+1,i_close-i_open-1);
    QStringList extensions = iopluginname.split(QRegExp(","));
    foreach(QString extension, extensions){
        int i_dot = extension.indexOf(".");
        extension = extension.mid(i_dot+1);
        // qDebug() << "extracted extension: " << extension;
        retval.append( extension.toLower() );
    }
    
    return retval;
}
    
QString failurecauses_qtplugin(
"\nPOSSIBLE FAILURE REASONS:\n"
"  1) plugin needs a DLL which cannot be found in the executable folder\n"
"  2) ... add your reason?");
QString failurecauses_starlabplugin(
"\nPOSSIBLE FAILURE REASONS:\n"
"  2) starlab PluginManager does not know how to load it\n"
"  3) ... add your reason?");

PluginManager::PluginManager(StarlabSettings* settings) : 
    _settings(settings)
{
    //pluginsDir=QDir(getPluginDirPath()); 
    // without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
    //qApp->addLibraryPath(getPluginDirPath());
    //qApp->addLibraryPath(getBaseDirPath());
    
    try 
    {
        loadPlugins();
    } 
    STARLAB_CATCH_BLOCK 
}

void PluginManager::loadPlugins() {
    // qDebug() << "PluginManager::loadPlugins(..)";
    
    pluginsDir=QDir(getPluginDirPath());
    // without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
    // qApp->addLibraryPath(getPluginDirPath());
    // qApp->addLibraryPath(getBaseDirPath());
    QStringList pluginfilters(LIB_EXTENSION_FILTER);
    //only the files with appropriate extensions will be listed by function entryList()
    pluginsDir.setNameFilters(pluginfilters);

    // qDebug( "Loading plugins from: %s ",qPrintable(pluginsDir.absolutePath()));
    
    /// Load all plugins
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        
        /// Attempt to load a Qt plugin
        QString path = pluginsDir.absoluteFilePath(fileName);
        QPluginLoader loader(path);
        QObject* plugin = loader.instance();
        if(!plugin) qDebug() << "Plugin load failure: " << fileName;
        if(!plugin) throw StarlabException("Plugin " + fileName + " is not a proper *Qt* plugin!! " + failurecauses_qtplugin);
        // qDebug() << "Loading plugin from file: " << fileName;
        
        /// Attempt to load one of the starlab plugins
        bool loadok = false;
        loadok |= load_InputOutputPlugin(plugin);
        loadok |= load_ProjectInputOutputPlugin(plugin);
        loadok |= load_FilterPlugin(plugin);       
        loadok |= load_DecoratePlugin(plugin);
        loadok |= load_GuiPlugin(plugin);        
        loadok |= load_EditPlugin(plugin);
        loadok |= load_RenderPlugin(plugin);
        if( !loadok ) 
            throw StarlabException("plugin "+fileName+" was not recognized as one of the declared Starlab plugin!!"); // +failurecauses_starlabplugin));
        
        StarlabPlugin* splugin = dynamic_cast<StarlabPlugin*>(plugin);
        if(!splugin) continue;
            
        /// Store pointers to all plugin
        _plugins.insert(splugin->name(),splugin);
        
        /// If we read here loading went ok
        // qDebug() << "Plugin: " << fileName << " loaded succesfully";
    }
}

QString PluginManager::getBaseDirPath(){
    QDir baseDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    // Windows:
    // during development with visual studio binary could be in the debug/release subdir.
    // once deployed plugins dir is in the application directory, so
    if (baseDir.dirName() == "debug" || baseDir.dirName() == "release")		
        baseDir.cdUp();
#endif

#if defined(Q_OS_MAC)
    /// In MAC application is always deployed as "Starlab.app/Contents/MacOS/starlab"
    /// So going 2 folders up we will find the plugin folder
    baseDir.cdUp();
    baseDir.cdUp();
#endif

    return baseDir.absolutePath();
}

QString PluginManager::getPluginDirPath() {
    QDir pluginsDir(getBaseDirPath());
    if(!pluginsDir.exists("plugins"))
        throw StarlabException("Serious error. Unable to find the plugins directory.");
    pluginsDir.cd("plugins");
    return pluginsDir.absolutePath();
}

bool PluginManager::load_ProjectInputOutputPlugin(QObject *plugin){
    ProjectInputOutputPlugin* iIO = qobject_cast<ProjectInputOutputPlugin*>(plugin);
    if(!iIO) return false;
    projectIOPlugins.push_back(iIO);
    
    QStringList exts = extractExtensions( iIO->name() );
    foreach(QString ext, exts)
        projectExtensionToPlugin.insert(ext,iIO);

    return true;
}

bool PluginManager::load_InputOutputPlugin(QObject *plugin){
    InputOutputPlugin* iIO = qobject_cast<InputOutputPlugin*>(plugin);
    if(!iIO) return false;
    modelIOPlugins.insert(iIO->name(), iIO);

    /// Parse the extension filter into extensions
    QStringList exts = extractExtensions( iIO->name() );
    foreach(QString ext, exts)
        modelExtensionToPlugin.insert(ext,iIO);
    
    return true;
}

bool PluginManager::load_FilterPlugin(QObject *plugin){
    FilterPlugin* iFilter = qobject_cast<FilterPlugin*>(plugin);
    if(!iFilter) return false;
    filterPlugins.insert(iFilter->name(), iFilter);
    return true;
}

bool PluginManager::load_DecoratePlugin(QObject *plugin){
    DecoratePlugin* iDecorate = qobject_cast<DecoratePlugin*>(plugin);
    if(!iDecorate) return false;
    decoratePlugins.insert(iDecorate->name(), iDecorate);
    return true;
}
bool PluginManager::load_GuiPlugin(QObject* _plugin){
    GuiPlugin* plugin = qobject_cast<GuiPlugin*>(_plugin);
    if(!plugin) return false;
    guiPlugins.insert(plugin->name(), plugin);
    return true;
}

bool PluginManager::load_EditPlugin(QObject* _plugin){
    ModePlugin* plugin = qobject_cast<ModePlugin*>(_plugin);
    if(!plugin) return false;
    editPlugins.insert(plugin->name(), plugin);    
    return true;
}
 
bool PluginManager::load_RenderPlugin(QObject *_plugin){
    RenderPlugin* plugin = qobject_cast<RenderPlugin*>(_plugin);
    if(!plugin) return false;
    renderPlugins.insert(plugin->name(), plugin);
    return true;
} 

QString PluginManager::getFilterStrings(){
    QStringList filters;
    
    /// Fill in filters for Model files
    /// @todo add the readable format name
    QStringList extensions= modelExtensionToPlugin.keys();
    foreach(QString extension, extensions)
        filters.append("*."+extension);
    
    /// Fill in filters for Document files
    filters.prepend("Starlab Project (*.starlab)");
    filters.prepend("All Files (*.*)");
    
    /// Merge in a ;; separated list
    return filters.join(";;");
}

RenderPlugin *PluginManager::newRenderPlugin(QString pluginName, Model* model){
    Q_ASSERT(model!=NULL);
    RenderPlugin *plugin = renderPlugins.value(pluginName,NULL);
    if(plugin==NULL) throw StarlabException("Attempted to load plugin '%s' but I couldn't find one loaded!!",qPrintable(pluginName));  
    RenderPlugin* newplugin = plugin->factory();
    newplugin->_mainWindow = plugin->_mainWindow;
    newplugin->_application = plugin->_application;
    newplugin->_model = model;
    /// If model deleted so is the renderer
    newplugin->setParent(model);
    /// Original action remains...
    newplugin->_action = plugin->_action;
    return newplugin;
}

QString PluginManager::getPreferredRenderer(Model *model){
    QString key = "DefaultRenderer/"+QString(model->metaObject()->className());
    QString rendererName = settings()->getString(key);

    /// Deal with a non-existent preferred plugin
    if(!renderPlugins.contains(rendererName)){
        rendererName = "Bounding Box";

        // Query renderers, if any request to be default
        foreach(RenderPlugin* plugin, renderPlugins)
        {
            if(plugin->isApplicable(model) && plugin->isDefault()){
                rendererName = plugin->name();
                break;
            }
        }
        settings()->set(key,rendererName);
    }

    return rendererName;
}

void PluginManager::setPreferredRenderer(Model *model, RenderPlugin* plugin){
    // qDebug("PluginManager::setPreferredRenderer(..,%s)",qPrintable(plugin->name()));
    Q_ASSERT(plugin!=NULL);
    QString key = "DefaultRenderer/"+QString(model->metaObject()->className());
    settings()->set(key,plugin->name());
    settings()->sync();
}

QList<QAction *> PluginManager::getRenderPluginsActions(Model* model){
    QList<QAction*> retval;
    if(model==NULL) return retval;
    foreach(RenderPlugin* plugin, renderPlugins.values()){
        if( plugin->isApplicable(model) )
            retval.append(plugin->action());
    }
    return retval;
}
