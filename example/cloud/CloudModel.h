#pragma once
#include <QDebug>
#include <QString>
#include "global.h"
#include "Model.h"

class EXPORT CloudModel : public Model{
    Q_OBJECT   

public:
    class Point{
    public:
        Point(float x, float y, float z){
            selected = false;
            coord.setX(x);
            coord.setY(y);
            coord.setZ(z);
        }
        QVector3D coord;
        bool selected;
    };

public:    
    /// 3D Point cloud + aux field
    QVector<Point*> points;

    // Cloud points properties
    QMap< QString, QVector<QVariant> > property;
    QVector<QVariant> & getProperty(QString name);
    
public:
    CloudModel(QString path, QString name=QString()) : Model(path,name){}
    void updateBoundingBox();
};
