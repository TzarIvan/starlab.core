#pragma once
#include <QDebug>
#include <QString>
#include "global.h"
#include "Model.h"

class RenderPlugin;
class EXPORT CloudModel : public Model{
    Q_OBJECT   
public:    
    /// 3D Point cloud + aux field
    QVector<QVector4D> points; 
    
public:
    CloudModel(QString path, QString name=QString()) : Model(path,name){}
    void updateBoundingBox();
    RenderPlugin* defaultRenderer();
};
