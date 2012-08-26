#pragma once
#include <QDebug>
#include <QString>
#include <QSharedPointer>
#include "global.h"
#include "Model.h"

class RenderPlugin;
class EXPORT CloudModel : public Model{
    Q_OBJECT   
public:    
    /// 3D Point cloud + aux field
    QVector<QVector4D> points;

    // Cloud points properties
    QMap< QString, QVector<QVariant> > property;
    QVector<QVariant> getProperty(QString name);
    
public:
    CloudModel(QString path, QString name=QString()) : Model(path,name){}
    void updateBoundingBox();
    RenderPlugin* defaultRenderer();
};
