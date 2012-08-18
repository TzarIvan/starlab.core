#pragma once
#include "StarlabPlugin.h"
#include "Model.h"

/// This is a factory class that generates 
class RenderPlugin : public StarlabPlugin{
    /// @{ available for the plugin
    public:
        /// @brief Render the model
        virtual void render()=0;
        /// @brief Can this plugin render the model?
        /// @note known: model!=NULL 
        virtual bool isApplicable(Model* model)=0;
        /// @brief Initialize render plugin (optional):
        /// - initialize GL memory buffer
        /// - read shader specification from file
        virtual void init(){}
        /// @brief Clean up resources (destructor)
        virtual void finalize(){}    
    protected:
        /// @brief returns the model associated with the plugin
        Model* model(){ return _model; }
        /// @brief Return a new instance of your plugin    
        virtual RenderPlugin* factory()=0;    
    /// @}
    
    /// @{ internal usage
    friend class PluginManager;
    private:
        /// @internal
        Model* _model;
    public:
        RenderPlugin():_model(NULL){}
    /// @}
};

Q_DECLARE_INTERFACE(RenderPlugin, "starlab.RenderPlugin/1.1")
