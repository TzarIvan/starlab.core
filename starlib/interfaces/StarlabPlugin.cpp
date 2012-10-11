#include <QPluginLoader>

#include "StarlabPlugin.h"
#include "StarlabMainWindow.h"

StarlabPlugin::StarlabPlugin(){ 
    _mainWindow = NULL;
    _application = NULL;
    _action = NULL; 
}

StarlabDrawArea* StarlabPlugin::drawArea(){ 
    if(_mainWindow==NULL) return NULL;
    return mainWindow()->drawArea(); 
}

StarlabMainWindow* StarlabPlugin::mainWindow(){ 
	return _mainWindow; 
}
 
StarlabApplication* StarlabPlugin::application(){ 
	return _application; 
}

Document* StarlabPlugin::document(){ 
	Q_ASSERT(_application);
	return _application->document(); 
}

StarlabSettings* StarlabPlugin::settings(){ 
	Q_ASSERT(_application);
	return _application->settings(); 
}

PluginManager* StarlabPlugin::pluginManager(){ 
	Q_ASSERT(_application);
	return _application->pluginManager();
}

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


