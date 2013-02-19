#pragma once 
#include <QObject>
#include <QtCore/qplugin.h>
#include <QVector>
#include <QAction>
#include <QKeySequence>

#include "StarlabException.h"
#include "Starlab.h"
#include "Model.h"
#include "Document.h"

class RichParameterSet;

using namespace Starlab;

/** 
 * @brief The superclass of any starlab plugin
 * @defgroup StarlabPluginInterfaces Starlab Plugin Interfaces
 */
class STARLIB_EXPORT StarlabPlugin : public QObject{
    Q_OBJECT

/// @{ basic plugin definition
public:
    /// Default constructor
    StarlabPlugin();

    /// The longer string describing each filtering action.
    /// => Printed at the top of the parameter window
    /// => You can use html formatting tags (like <br> <b> and <i>) to improve readability.
    /// => Used in the "About plugin" and to create the filter list page.
    virtual QString description() { return name(); }

    /// The very short string (a few words) describing each filtering action
    /// This string is used also to define the menu entry. The defaut name
    /// is the one of the plugin filename which is set by the pluginloader
    /// at boot time.
    virtual QString name() { return metaObject()->className(); }
    
    /// If the plugin has an icon, return it.
    virtual QIcon icon() { return QIcon(); }
    
    /// What shortcut would you like me to use for this plugin?
    virtual QKeySequence shortcut(){ return QKeySequence(); }
/// @}
    
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
    Document*           document();
    Application* application();
    Settings*    settings();
    PluginManager*      pluginManager();
    MainWindow*  mainWindow();
    DrawArea*    drawArea();
private:
    friend class Starlab::Application;
    friend class Starlab::MainWindow;
    friend class Starlab::PluginManager;
    Application* _application;
    MainWindow* _mainWindow;
/// @}
        
/// @{ friendship trick
    friend class GuiPlugin;
    friend class InputOutputPlugin;
    friend class ProjectInputOutputPlugin;
    friend class FilterPlugin;
    friend class DecoratePlugin;
    friend class ModePlugin;
    friend class RenderPlugin;
/// @}

/// @{ logging system
public:
    /// Maybe use flags for multiple usage?
    // enum LOGTYPE{STATUSBAR, TERMINAL};
    // void Log(int Level, const char * f, ... ) ;
public:
    void showMessage(const char *format, ...);
    void showProgress(double val);
/// @}
};
