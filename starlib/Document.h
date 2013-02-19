#pragma once
#include "Model.h"
#include "StarlabException.h"
#include "starlib_global.h"

namespace Starlab{

class STARLIB_EXPORT Document : public QObject{
    Q_OBJECT 
    
/// @{ Busy system management
private:
    int _isBusy;     /// defaults 0
public:
    void pushBusy(); ///< Increases the busy level of the document 
    void popBusy();  ///< Decreases the busy status, emits hasChanged() if !isBusy
    bool isBusy();   /// Is the document busy? (i.e. busy during I/O)   
/// @}

private:
    /// The list of all models in the document
    QList<Model*> _models;     
    
public:
    /// The name of the document, used as label
    QString name;
    /// Filepath where the document is saved (or will be...)
    QString path;    
    /// Has document been modified? (i.e. added/removed a model)
    bool isModified;
    /// Viewpoint transformation (identifies trackball)
    /// @todo use the QGLViewer transformation datatype
    QMatrix4x4 transform;

/// @{ Selection management
private:
    /// the model which is currently selected
    Model* _selectedModel;
public:
    /// Retrieves the selection, exception thrown with empty selection
    Model* selectedModel();
    /// Updates the selection, making sure this is a valid model
    void setSelectedModel(Model* model);
    /// @todo Allow for multiple Selection
    // QList<Model*> selectedModels();
/// @}

public:
    /// Basic document constructor
    Document();
    /// Compute bounding box of all the elements in the scene
    QBox3D bbox();
    /// Add a model (if it's only one becomes selection)
    void addModel(Model* m);
    /// Clean slate, remove all models
    void clear();
    /// Replace Model, also deleting the memory of the old
    void replaceModel(Model* old_model, Model* new_model);
    /// Is there any model?
    bool isEmpty(){ return _models.empty(); }
    /// The models in this layer (cannot edit the set directly!)
    const QList<Model*>& models(){ return _models; }
    
/// @{ Document status updates
signals:
    void resetViewport();                ///< Document requesting a reset of the view matrixes
    void hasChanged();                   ///< Document has changed, emitted after "last" popBusy()
    void deleteScheduled(Model*);        ///< This model is scheduled for deletion
    void selectionChanged(Model* model); ///< The selection has changed to the provided model
public:
    void emit_resetViewport();
private:
    void emit_hasChanged();
/// @}
};

} // namespace
