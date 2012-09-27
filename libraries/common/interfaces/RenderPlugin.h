#pragma once
#include "StarlabPlugin.h"
#include "Model.h"

/// This is a factory class that generates 
class RenderPlugin : public StarlabPlugin{
    
/// @{ RenderPlugin interface
public:
    /// @brief Can this plugin render the given model?
    virtual bool isApplicable(Model* model)=0;

    /// @brief Render the model, default implementation only sets glColor
    virtual void render() = 0;    
    
    /// @brief Initialize render plugin (optional), for example
    /// - initialize GL memory buffer
    /// - read shader specification from file
    /// @todo rename into create
    virtual void init(){}
    
    /// @brief Clean up resources (the ones you allocated in init())
    /// @todo rename into destroy
    virtual void finalize(){}    
    
    /// @brief returns the model that this plugin should be rendering
    Model* model(){ return _model; }

    /// @brief Overload it and return a new instance of your plugin    
    virtual RenderPlugin* factory()=0;

    /// @brief Overload and return true if you would like this plugin to be a default
    virtual bool isDefault() { return false; }
/// @}
    
/// @{ Internal Usage
private:
    Model* _model;
friend class PluginManager;
/// @}
};

Q_DECLARE_INTERFACE(RenderPlugin, "starlab.RenderPlugin/2.0")
