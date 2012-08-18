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
    // This will disappear with the new C++ standard
    ModePlugin() { isSuspended=false; }
    bool isSuspended;    
    
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
       
    /// @{ @name Decorating events (optional)
    public: 
        virtual void Decorate(QPainter* /*p*/){}
    /// @} 

    /// @{ @name User Input Callbacks (optional)
    public: 
        virtual bool mousePressEvent    (QMouseEvent*  /*event*/) { return false; }
        virtual bool mouseMoveEvent     (QMouseEvent*  /*event*/) { return false; }
        virtual bool mouseReleaseEvent  (QMouseEvent*  /*event*/) { return false; }
        virtual bool keyReleaseEvent    (QKeyEvent*    /*event*/) { return false; }
        virtual bool keyPressEvent      (QKeyEvent*    /*event*/) { return false; }
        virtual bool tabletEvent        (QTabletEvent* /*event*/) { return false; }
        virtual bool wheelEvent         (QWheelEvent*  /*event*/) { return false; }
    /// @}
};

Q_DECLARE_INTERFACE(ModePlugin, "starlab.ModePlugin/1.0")
