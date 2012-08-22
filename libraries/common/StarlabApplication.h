#pragma once
#include <QApplication>
#include <QMessageBox>
#include "Document.h"
#include "PluginManager.h"
#include "StarlabSettings.h"
#include "dynamic_common_global.h"

/// @note QApplication is for GUI, QCoreApplication is for command line stuff
class DYNAMIC_COMMON_EXPORT StarlabApplication : public QApplication{
    
    /// @{ core resources and accessors
    public:
        PluginManager* pluginManager(){ return _pluginManager; }
        Document* document(){ return _document; }
        StarlabSettings* settings(){ return _settings; }
    private:
        PluginManager*   _pluginManager; /// The plugins system
        Document*        _document;      /// The set of loaded models
        StarlabSettings* _settings;      /// The application settings
    /// @}
        
public:
    StarlabApplication(int &argc, char* argv[]);   
    ~StarlabApplication();
    /// @todo why is this necessary?
    bool notify( QObject * rec, QEvent * ev );
    
public:
    /// @{ I/O: return false if don't know how to load
        void load(QString path);
        bool loadModel(QString path, InputOutputPlugin* plugin=NULL);
        bool saveModel(Model* model, QString path="");
        bool loadProject(QString path, ProjectInputOutputPlugin* plugin=NULL);
    /// @}
    
    void executeFilter(QString filterName);
    
    /// Where is Starlab located?
    /// Mac: /Applications/Starlab.app
    /// Win: C:/Program Files/starlab
    /// Lin: ????
    QString starlabDirectory();
};
