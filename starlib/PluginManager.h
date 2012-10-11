#pragma once

#include <QMap>
#include <QObject>
#include <QDir>
#include "starlib_global.h"

/// Forward declarations
class DrawAreaPlugin;
class RenderPlugin;
class DecoratePlugin;
class FilterPlugin;
class GuiPlugin;
class ModePlugin;

/// @todo why am I not allowed to forward declare these?
#include "interfaces/InputOutputPlugin.h"
#include "interfaces/ProjectInputOutputPlugin.h"

/// \brief This class provides the basic tools for managing all the plugins. It is used by both meshlab and meshlab server.
class STARLIB_EXPORT PluginManager: public QObject{
    Q_OBJECT
    
    /// @{ constructor
    public:
        PluginManager(StarlabSettings *settings);
    private:
        StarlabSettings* _settings;
        StarlabSettings* settings(){ return _settings; }
    /// @}
        
    /// @{ Stores all (loaded) plugins
    private:
        QMap<QString,StarlabPlugin*> _plugins;
    public:
        QList<StarlabPlugin*> plugins(){ return _plugins.values(); }
    /// @}
        
public:  
    /// Stores the loaded IO model plugins
    /// @todo necessary? just use the values of the hash map 
    QMap<QString,InputOutputPlugin*> modelIOPlugins;
    /// Extension (lowercase) => IO model plugin
    QHash<QString,InputOutputPlugin*> modelExtensionToPlugin;
    
    /// Stores the loaded IO project plugins
    /// @todo necessary? just use the values of the hash map 
    QVector<ProjectInputOutputPlugin*> projectIOPlugins;   
    /// Extension (lowercase) => IO project plugin
    QHash<QString,ProjectInputOutputPlugin*> projectExtensionToPlugin;

    /// @{ Store the loaded plugins by name
        QMap<QString,GuiPlugin*> guiPlugins;
        QMap<QString,FilterPlugin*> filterPlugins;
        QMap<QString,ModePlugin*> editPlugins;
        QMap<QString,DecoratePlugin*> decoratePlugins;
    /// @}


    /// @{ Render Plugins Control
    private:
        QMap<QString,RenderPlugin*> renderPlugins;        
    public:
        /// Creates a new instance of a render plugin
        /// @exception if plugin with that name cannot be found
        RenderPlugin* newRenderPlugin(QString pluginName);
        QString getPreferredRenderer(Model* model);
        void setPreferredRenderer(Model* model, RenderPlugin* pluginName);
        
        /// Get the list of actions corresponding to render plugin that apply to this type of model
        QList<RenderPlugin*> getApplicableRenderPlugins(Model *model);
    /// @}

    /// @{ Decorate plugin control
        DecoratePlugin *newDecoratePlugin(QString pluginName, Model *model);
    /// @}
        
public:
    /// Directory where plugins are found
    QDir pluginsDir;    
    static QString getPluginDirPath();
    
    /// Gives path to the root of the applicative
    /// In MAC: /path/to/Starlab.app/
    /// In WIN: /path/to/Starlab/
    static QString getBaseDirPath();
    
    /// Assemble string for file open/save filters
    QString getFilterStrings();
    
private:
    /// Searches pluginsDir for plugins
    void loadPlugins();

/// Set of helper functions
private:
    bool load_InputOutputPlugin(QObject* plugin);
    bool load_ProjectInputOutputPlugin(QObject* plugin);
    bool load_FilterPlugin(QObject* plugin);
    bool load_EditPlugin(QObject* plugin);
    bool load_DecoratePlugin(QObject* plugin);    
    bool load_GuiPlugin(QObject* _plugin);
    bool load_DrawAreaPlugin(QObject* _plugin);
    bool load_RenderPlugin(QObject* _plugin);
};
