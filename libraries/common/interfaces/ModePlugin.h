#pragma once 
#include <QObject>
#include <QDockWidget>
#include "StarlabPlugin.h"
#include "RichParameterSet.h"
#include "Callbacks.h"
#include "Document.h"
#include "StarlabException.h"
class StarlabDrawArea;

/**
 * @brief The interface of a plugin which gives you total control over the GUI.           
 * @ingroup StarlabPluginInterfaces 
 */
class ModePlugin : public StarlabPlugin {
public: 
    ModePlugin(){ _model = NULL; }

    /// @brief can the plugin be used?
    virtual bool isApplicable(Document* doc) = 0;
    
    /// @{ @name Plugin Creation/Termination
    public:
        /// Called when the user starts the edit the FIRST time (i.e. it does not get re-loaded when the plugin is suspended)
        /// Think about it as a constructor, as the plugin constructor would be automatically loaded at boot time.
        virtual void createEdit()=0;
        /// Called when the user closes the edit
        /// Automatically closes the widget
        /// @todo is suspend a close?
        virtual void destroyEdit()=0;
    /// @} 
       
    /// @{ @name Rendering events (optional)
    public: 
        virtual void decorate(){}
        virtual void drawWithNames(){}
    /// @} 

    /// @{ @name Selection events (optional)
    public:
        virtual void endSelection(const QPoint&){}
        friend class StarlabDrawArea;
    /// @}

    /// @{ @name User Input Callbacks (optional)
    public: 
        virtual bool mousePressEvent    (QMouseEvent* ) { return false; }
        virtual bool mouseMoveEvent     (QMouseEvent* ) { return false; }
        virtual bool mouseReleaseEvent  (QMouseEvent* ) { return false; }
        virtual bool keyReleaseEvent    (QKeyEvent*   ) { return false; }
        virtual bool keyPressEvent      (QKeyEvent*   ) { return false; }
        virtual bool tabletEvent        (QTabletEvent*) { return false; }
        virtual bool wheelEvent         (QWheelEvent* ) { return false; }
    /// @}
    
    /// @{ Access to properties
    protected:
        using StarlabPlugin::drawArea;
        using StarlabPlugin::document;
    /// @} 
    
    /// @{ 
    public:
        /// @brief returns the model associated with the plugin
        Model* model(){
            if(!_model) _model = document()->selectedModel();
            return _model;
        }
    friend class PluginManager;
    private:
        Model* _model;
    /// @}
};

Q_DECLARE_INTERFACE(ModePlugin, "starlab.ModePlugin/1.0")
