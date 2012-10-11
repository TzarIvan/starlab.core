#pragma once 
#include <QFileInfo>
#include <QObject>
#include "StarlabPlugin.h"
#include "Model.h"
#include "StarlabException.h"
#include "Document.h"

class StarlabApplication;
class RichParameterSet;

/** 
 * @brief Plugin interface to perform I/O of project files
 * @ingroup StarlabPluginInterfaces 
 * 
 * These plugins are responsible for the I/O of a single model.
 * A plugin is associated with one and only one extension.
 */
class ProjectInputOutputPlugin : public StarlabPlugin{
   
public:
    /// Provide the path of the project to open. Resources (loadModel) to open 
    /// the content of the project are provided by StarlabApplication
    virtual bool open(const QString& path, StarlabApplication& application) = 0;
    /// @todo document
    virtual void save(const QString& path, Document& document) = 0;
};

Q_DECLARE_INTERFACE(ProjectInputOutputPlugin, "starlab.ProjectInputOutputPlugin/1.0")
