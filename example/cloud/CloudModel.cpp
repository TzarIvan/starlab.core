#include "CloudModel.h"

void CloudModel::updateBoundingBox(){
    _bbox.setToNull();
    foreach(QVector4D p, points)
        _bbox.unite(QVector3D(p.x(),p.y(),p.z()));      
}
