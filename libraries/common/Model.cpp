#include "Model.h"

#include <QTreeWidgetItem>  /// Plugins define items for layer tree menu
#include <QMenu>
#include <QToolBar>
#include <QAction>          /// Plugins define actions for toolbar buttons and such
#include <QFileInfo>        /// To automatically determine basename

#include "StarlabException.h"

Model::Model(QString path, QString name){
    this->path = "";
    this->name = "";
    this->color = Qt::gray;
    this->isVisible = true;
    this->isModified = false;
    
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
