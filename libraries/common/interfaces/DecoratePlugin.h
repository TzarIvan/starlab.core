#pragma once 
#include "Model.h"
#include "StarlabPlugin.h"
class StarlabDrawArea;

/**
 * @brief Sits on top of render plugin, example: render surface normals
 * @note  #include "StarlabDrawArea.h" in any plugin that inherits...
 * @ingroup StarlabPluginInterfaces 
 */
class DecoratePlugin : public StarlabPlugin{
public:
    /// This is similar to a render loop, thus it's called repeteadly
    virtual void decorate(Model* /*model*/, StarlabDrawArea* /*parent*/, QPainter* /*p*/) = 0;
    /// Allocate/Initialize anything needed in here
    virtual void startDecorate(Model* /*model*/, StarlabDrawArea* /*parent*/){}
    /// Free resources?
    virtual void endDecorate(Model* /*model*/, StarlabDrawArea* /*parent*/) {}
    
public:
    /// @internal A decorator plugin can either be enabled or disabled thus we make its action "Checkable"
    QAction* action(){
        QAction* action = StarlabPlugin::action();
        action->setCheckable(true);
        action->setChecked(false);
        return action;
    }
};

Q_DECLARE_INTERFACE(DecoratePlugin, "starlab.DecoratePlugin/1.0")
