#pragma once 
#include <QObject>
#include <QtCore/qplugin.h>
#include <QVector>
#include <QAction>
#include <QKeySequence>

#include "StarlabException.h"
#include "Model.h"
#include "Document.h"

class RichParameterSet;
class StarlabDrawArea;
class StarlabApplication;
class StarlabMainWindow;
class StarlabSettings;
class PluginManager;

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
    friend class GuiPlugin;
    friend class InputOutputPlugin;
    friend class ProjectInputOutputPlugin;
    friend class FilterPlugin;
    friend class DecoratePlugin;
    friend class ModePlugin;
    friend class RenderPlugin;
/// @}

/// @{ logging system
signals:
    void logmessage(QString message);
    void logprogress(QString origin, float perc);
public:
    /// Maybe use flags for multiple usage?
    // enum LOGTYPE{STATUSBAR, TERMINAL};
    // void Log(int Level, const char * f, ... ) ;
public:
    void Log(const char *format, ...){
        const size_t buffer_length=256;
        char buffer[buffer_length];
        va_list args;
        va_start (args, format);
        vsnprintf(buffer,buffer_length,format, args);
        va_end (args);
		QString msg(buffer);
        qDebug() << msg;
        emit logmessage( msg );
    }
    void Progress(float val){
        emit logprogress(this->name(), val);
    }
/// @}

};
