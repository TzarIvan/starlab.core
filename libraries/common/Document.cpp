#include "Document.h"
#include <QFileInfo>
#include <QDir>

Document::Document(){
    /// @todo does EIGEN offer a way of doing this?
    transform.setToIdentity();
    _selectedModel = NULL;
    name = "Unsaved Project";
	_isBusy = 0;
}

QBox3D Document::bbox() {
    QBox3D box;
    /// @todo rotate bbox of model by its custom transfrom like meshlab was doing in box.Add(m->transform,m->bbox)
    foreach(Model* m, _models)
        box.unite(m->bbox());
    return box;
}

void Document::addModel(Model* m){
    if(m==NULL) throw StarlabException("Attempted to add a NULL model to the document");
    _models.append(m);

    /// First model added is selection
    if(_models.size()==1)
        setSelectedModel(m); 
    
    /// Tell others I have changed
    emit hasChanged();
}

void Document::clear(){
    /// Delete models individually
    foreach(Model* model, _models)
        model->deleteLater();
    /// Clear the list
    _models.clear();
    /// Clear the selection
    _selectedModel = NULL;
}

void Document::replaceModel(Model *old_model, Model *new_model){
    // qDebug() << "Document::replaceModel(" << old_model->path << "," << new_model->path << ")";
    /// Search old in list
    int index = _models.indexOf(old_model);
    Q_ASSERT(index!=-1);
    _models[index] = new_model;
    delete old_model;
}

Model* Document::selectedModel(){
    return _selectedModel;
}

void Document::setSelectedModel(Model* model){
    if(model==NULL) throw StarlabException("Requested selection update with a 'NULL' model");
    bool exists = _models.contains(model);
    if(!exists) throw StarlabException("Requested selection update with a model which is not part of the document");
    _selectedModel = model;
}
