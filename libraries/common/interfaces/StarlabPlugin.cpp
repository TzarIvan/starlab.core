#include <QPluginLoader>

#include "StarlabPlugin.h"
#include "StarlabMainWindow.h"

StarlabPlugin::StarlabPlugin(){ _action = NULL; }

StarlabMainWindow* StarlabPlugin::mainWindow(){ return _mainWindow; }
StarlabDrawArea* StarlabPlugin::drawArea(){ return mainWindow()->drawArea(); }
    
StarlabApplication* StarlabPlugin::application(){ return _application; }
Document* StarlabPlugin::document(){ return application()->document(); }
StarlabSettings* StarlabPlugin::settings(){ return application()->settings(); }
PluginManager* StarlabPlugin::pluginManager(){ return application()->pluginManager(); }
Model *StarlabPlugin::model(){ return document()->selectedModel(); }

/// @internal action cannot be created in constructor. This is because 
/// in the constructor local methods are preferred over polymorphic ones :(
QAction *StarlabPlugin::action(){ 
    if(_action==NULL){
        _action = new QAction(icon(),name(),NULL);
        _action->setToolTip(description());
        _action->setShortcut(shortcut());
        _action->setParent(this);        
    }
    return _action;
}


