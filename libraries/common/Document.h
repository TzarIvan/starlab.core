#pragma once
#include "Model.h"
#include "StarlabException.h"
#include "dynamic_common_global.h"

class DYNAMIC_COMMON_EXPORT Document : public QObject{
    Q_OBJECT 
    
/// @{ Busy system management
private:
    int _isBusy; // default 0
public:
    void pushBusy(){_isBusy++;}
    void popBusy(){if(_isBusy>=1) _isBusy--;}
    /// Is the document busy? (I/O for example)
    bool isBusy(){ return _isBusy>0; }
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
    
    /// Retrieves a model by name 
    /// @todo move to cpp file
    template<class T> T* getModel(QString name){
        foreach(Model* model, _models)
            if(model->name == name){
                T* retval = qobject_cast<T*>(model);
                if(!retval) 
                    throw StarlabException("A model named '%s' of the required type was not found", qPrintable(name));
                return retval;
            }        
        throw StarlabException("Requested model '%s'' not found",qPrintable(name));
    }

/// @{ Document status updates
signals:
    void resetViewport(); ///< Document requesting a reset of the view matrixes
    void hasChanged();    ///< Document has changed
public:
    void emit_resetViewport(){ emit resetViewport(); }
    void emit_hasChanged(){ emit hasChanged(); }
/// @}
    
public: 
    /// Converts document to XML and writes to the desired file
    void writeToXML(QString filename);
    void readFromXML(QString filename);
};
