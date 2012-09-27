#include "CloudModel.h"

void CloudModel::updateBoundingBox(){
    _bbox.setToNull();
    foreach(Point* p, points)
        _bbox.unite(p->coord);
}

QVector<QVariant>& CloudModel::getProperty(QString name)
{
    // Initialize data if it is missing
    if(!property.keys().contains(name))
        property[name] = QVector<QVariant>(points.size());

    return property[name];
}
