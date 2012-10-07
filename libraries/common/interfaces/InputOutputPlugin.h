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
 * @ingroup StarlabPluginInterfaces 
 * 
 * These plugins are responsible for the I/O of a single model.
 */
class InputOutputPlugin : public StarlabPlugin{
   
/// @{ basic interface
public:
    /// In InputOutputPlugin the name of the plugin is used to construct the 
    /// string you will find in the save/load GUI. The format has to be something like:
    /// "[CGALMesh] Object File Format (*.off)"
    virtual QString name() = 0;

    /// can this plugin Save/Load the model?
    virtual bool isApplicable(Model*) = 0;
    
    /// Opens a model at the specified path
    /// @note existance and readability of the file at path is pre-checked
    virtual Model* open(QString path) = 0;
    
    /// Saves a model to the specified path
    virtual void save(Model*, QString path) = 0;    
/// @}
    
/// @{ utility functions
protected:
    /// use this in your plugins to your advantage
    QString pathToName(QString path){
        QFileInfo fi(path);
        return fi.completeBaseName();
    }
private:
    friend class StarlabApplication;
    /// This is called before InputOutputPlugin::open(...), no need to do check existance yourself
    void checkReadable(QString path){
        QFileInfo fi(path);
        QString absPath = fi.absoluteFilePath();
        if(!fi.exists()) throw StarlabException("File at: '%s' does not exist.",qPrintable(absPath));
        if(!fi.isReadable()) throw StarlabException("File at: '%s' does not exist.",qPrintable(absPath));            
    }
/// @}
};

Q_DECLARE_INTERFACE(InputOutputPlugin, "starlab.InputOutputPlugin/1.0")
