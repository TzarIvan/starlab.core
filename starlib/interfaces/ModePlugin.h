#pragma once 
#include "StarlabPlugin.h"

/**
 * @brief The interface of a plugin which gives you total control over the GUI.           
 * @ingroup stariface 
 *
 * @note If you plan to create a widget you should use a ModePluginDockWidget
 */
class ModePlugin : public StarlabPlugin {
public: 
    /// @brief can the plugin be used?
    virtual bool isApplicable() = 0;
    
    /// @{ @name Plugin Creation/Termination
    public:
        /// Called when the user starts the edit the FIRST time (i.e. it does not get re-loaded when the plugin is suspended)
        /// Think about it as a constructor, as the plugin constructor would be automatically loaded at boot time.
        virtual void create()=0;
        /// Called when the user closes the edit
        /// Automatically closes the widget
        /// @todo is suspend a close?
        virtual void destroy()=0;

        /// Called whenever the document is changed 
        /// If you return false (or if you don't overload this function) we will 
        /// just call ModePlugin::destory() and ModePlugin::create() in succession.
        virtual bool documentChanged(){ return false; }
    /// @} 

    /// @{ @name Support for suspension/resume
    public:
        /// Called whenever the plugin is suspended (i.e. ESCAPE)
        /// If you created GUIs, hide or disable them here
        virtual void suspend(){}
        /// Called whenever the plugin is resumed (after a suspend)
        /// If you created GUIs, hide or disable them here
        virtual void resume(){}
    /// @}
    
    /// @{ @name Rendering events (optional)
    public: 
        virtual void decorate(){}
        virtual void drawWithNames(){}
    /// @} 

    /// @{ @name Selection events (optional)
    public:
        virtual void endSelection(const QPoint&){}
        virtual void postSelection(const QPoint&){}
    /// @}

    /// @{ @name User Input Callbacks (optional)
    public: 
        virtual bool mousePressEvent        (QMouseEvent* ) { return false; }
        virtual bool mouseMoveEvent         (QMouseEvent* ) { return false; }
        virtual bool mouseReleaseEvent      (QMouseEvent* ) { return false; }
        virtual bool keyReleaseEvent        (QKeyEvent*   ) { return false; }
        virtual bool keyPressEvent          (QKeyEvent*   ) { return false; }
        virtual bool tabletEvent            (QTabletEvent*) { return false; }
        virtual bool wheelEvent             (QWheelEvent* ) { return false; }
        virtual bool mouseDoubleClickEvent  (QMouseEvent* ) { return false; }
    /// @}
    
    /// @{ Access to properties
    protected:
        using StarlabPlugin::drawArea;
        using StarlabPlugin::document;
        using StarlabPlugin::mainWindow;
        using StarlabPlugin::application;
        using StarlabPlugin::pluginManager;
        Starlab::Model* selection(){ return document()->selectedModel(); }    
    /// @}
};

Q_DECLARE_INTERFACE(ModePlugin, "Starlab::ModePlugin/2.0")
