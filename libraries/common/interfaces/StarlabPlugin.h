#pragma once 
#include <QObject>
#include <QtCore/qplugin.h>
#include <QVector>
#include <QAction>
#include <QKeySequence>
#include "Model.h"
#include "Document.h"
#include "Callbacks.h"
#include "Logger.h"

class RichParameterSet;
class StarlabDrawArea;
class StarlabApplication;
class StarlabMainWindow;
class StarlabSettings;
class PluginManager;
class Document;

/** 
 * @brief The superclass of any starlab plugin
 *
 * @defgroup StarlabPluginInterfaces Starlab Plugin Interfaces
 */
class DYNAMIC_COMMON_EXPORT StarlabPlugin : public QObject, public Logger{
public:
    /// Default constructor
    StarlabPlugin();

    /// The longer string describing each filtering action.
    /// => Printed at the top of the parameter window
    /// => You can use html formatting tags (like <br> <b> and <i>) to improve readability.
    /// => Used in the "About plugin" and to create the filter list page.
    virtual QString description() { return "description not available"; }

    /// The very short string (a few words) describing each filtering action
    /// This string is used also to define the menu entry. The defaut name
    /// is the one of the plugin filename which is set by the pluginloader
    /// at boot time.
    virtual QString name() { return metaObject()->className(); }
    
    /// If the plugin has an icon, return it.
    virtual QIcon icon() { return QIcon(); }

    /// @{ @note Given the current Model/Document, can the plugin be executed? 
        virtual bool isApplicable(){ return true; }
    /// @}
    
    /// What shortcut would you like me to use for this plugin?
    virtual QKeySequence shortcut(){ return QKeySequence(); }
    
    /// @{ action management
    public:
        /// @brief assembles an action from name+icon
        /// @note the action's parent is the plugin!!
        QAction* action(); 
    protected:
        QAction* _action;
    /// @}
        
    /// @{ Quick access to the Starlab resources
    private:
        Model*              model();
        Document*           document();
        StarlabApplication* application();
        StarlabSettings*    settings();
        PluginManager*      pluginManager();
        StarlabMainWindow*  mainWindow();
        StarlabDrawArea*    drawArea();
    private:
        friend class StarlabApplication;
        friend class StarlabMainWindow;
        friend class PluginManager;
        StarlabApplication* _application;
        StarlabMainWindow* _mainWindow;
    /// @}
        
    /// @{ friendship trick
        friend class DecoratePlugin;
        friend class DrawAreaPlugin;
        friend class FilterPlugin;
        friend class GuiPlugin;
        friend class InputOutputPlugin;
        friend class ModePlugin;
        friend class ProjectInputOutputPlugin;
        friend class RenderPlugin;
    /// @}
};
