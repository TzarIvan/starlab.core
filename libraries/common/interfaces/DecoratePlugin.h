#pragma once 
#include "StarlabPlugin.h"

/**
 * Decorate your rendering with extra stuff (i.e. normals, selection, etc..)
 * @ingroup StarlabPluginInterfaces 
 */
class DecoratePlugin : public StarlabPlugin{
/// @{ DecoratePlugin interface
public:
    /// Overload it and return a new instance of your plugin    
    virtual DecoratePlugin* factory()=0;  
    
    /// Given the current Model, can the plugin be executed? 
    virtual bool isApplicable(Model*) = 0;
    
    /// This is similar to a render loop, thus it's called repeteadly
    virtual void decorate() = 0;
    
    /// Constructor (allocate resources)
    virtual void create(){}
    
    /// Destructor (free resources)
    virtual void destroy() {}
    
    /// returns the model that this plugin should be decorating
    Model* model(){ return _model; }
/// @}    

/// @{ Internal Usage
public:
    /// @internal A decorator plugin can either be enabled or disabled thus we make its action "Checkable"
    QAction* action(){
        QAction* action = StarlabPlugin::action();
        action->setCheckable(true);
        action->setChecked(false);
        return action;
    }
private:
    /// @internal Allow it to set _model
    friend class PluginManager;
    /// @internal The model that this plugin is decorating
    Model* _model;
/// @}
};

Q_DECLARE_INTERFACE(DecoratePlugin, "starlab.DecoratePlugin/2.0")
