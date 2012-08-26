#include "CloudModel.h"

void CloudModel::updateBoundingBox(){
    _bbox.setToNull();
    foreach(QVector4D p, points)
        _bbox.unite(QVector3D(p.x(),p.y(),p.z()));
}

QVector<QVariant> CloudModel::getProperty(QString name)
{
    // Initialize data if it is missing
    if(!property.keys().contains(name))
        property[name] = QVector<QVariant>(points.size());

    return property[name];
}
