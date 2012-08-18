#pragma once

#include "vcg/space/box3.h"
#include "vcg/math/matrix44.h"

#include "qt3d/math3d/qbox3d.h"

vcg::Point3f eigen_2_vcg(QVector3D vec){
    return vcg::Point3f(vec.x(),vec.y(),vec.z());
}

/// Directly uses memory of Eigen 
vcg::Matrix44d eigen_2_vcg(QMatrix4x4 matrix){
    return vcg::Matrix44d( matrix.data() );
}

vcg::Box3f eigen_2_vcg(QBox3D _box){
    vcg::Box3f box;
    box.min = eigen_2_vcg(_box.minimum());
    box.max = eigen_2_vcg(_box.maximum());
    return box;
}
