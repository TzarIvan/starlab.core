#include "Model.h"

#include <QTreeWidgetItem>  /// Plugins define items for layer tree menu
#include <QMenu>
#include <QToolBar>
#include <QAction>          /// Plugins define actions for toolbar buttons and such
#include <QFileInfo>        /// To automatically determine basename

#include "StarlabException.h"
#include "interfaces/RenderPlugin.h"

Model::Model(QString path, QString name){
    this->path = "";
    this->name = "";
    this->color = Qt::gray;
    this->isVisible = true;
    this->isModified = false;
    this->_renderer = NULL;
    this->path = path;
    this->name = name.isEmpty() ? QFileInfo(path).baseName() : name;    
}

void Model::decorateLayersWidgedItem(QTreeWidgetItem* parent){
    QTreeWidgetItem *fileItem = new QTreeWidgetItem();
    fileItem->setText(1, QString("Path"));    
    fileItem->setText(2, this->path);
    parent->addChild(fileItem);
    // updateColumnNumber(fileItem);
}

RenderPlugin *Model::renderer(){
    return _renderer;
}

void Model::setRenderer(RenderPlugin* plugin){
    Q_ASSERT(plugin->isApplicable(this));
    if(_renderer != NULL){
        _renderer->finalize();
        _renderer->deleteLater();
    }
    /// Link it to this model
    plugin->_model = this;
    plugin->setParent(this);
    _renderer = plugin;
}
