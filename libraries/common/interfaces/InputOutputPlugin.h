#pragma once 
#include <QFileInfo>
#include <QObject>
#include "Callbacks.h"
#include "StarlabPlugin.h"
#include "Model.h"
#include "StarlabException.h"
#include "Document.h"

class StarlabApplication;
class RichParameterSet;

/** 
 * @brief todo
 * @ingroup StarlabPluginInterfaces 
 * 
 * These plugins are responsible for the I/O of a single model.
 * A plugin is associated with one and only one extension.
 */
class InputOutputPlugin : public StarlabPlugin{
   
public:
    /// @brief In InputOutputPlugin the name of the plugin is used to construct the 
    /// string you will find in the save/load GUI. The format has to be something like:
    /// "[CGALMesh] Object File Format (*.off)"
    virtual QString name() = 0;
    /// @brief can this plugin Save/Load the model?
    virtual bool isApplicable(Model*) = 0;
    /// @brief Opens a model at the specified path
    virtual Model* open(QString path) = 0;
    /// @brief Saves a model to the specified path
    /// @internal path 
    virtual void save(QString /*path*/, Model* /*model*/){ 
        throw StarlabException("InputOutputPlugin::save not implemented");
    }
    
    
    /// @{ utility functions: use these in your plugins to your advantage
    protected:
        QString pathToName(QString path){
            QFileInfo fi(path);
            return fi.completeBaseName();
        }
        void checkValidPath(QString path){
            QFileInfo fi(path);
            QString absPath = fi.absoluteFilePath();
            if(!fi.exists()) throw StarlabException("File at: '%s' does not exist.",qPrintable(absPath));
            if(!fi.isReadable()) throw StarlabException("File at: '%s' does not exist.",qPrintable(absPath));            
        }
    /// @}
};

Q_DECLARE_INTERFACE(InputOutputPlugin, "starlab.InputOutputPlugin/1.0")
