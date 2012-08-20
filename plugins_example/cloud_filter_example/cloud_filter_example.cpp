#include "cloud_filter_example.h"
#include "CloudModel.h"
#include "StarlabDrawArea.h"

QString printBounding(QBox3D box){
    QString retval;
    QTextStream sout(&retval);
    QVector3D c = box.center();
    QVector3D s   = box.size();
    sout << "Center[" << c.x() << " " << c.y() << " " << c.z() << "]" 
         << "  Size[" << s.x() << " " << s.y() << " " << s.z() << "]";
    return retval;
}

void cloud_filter_example::initParameters(RichParameterSet* pars){
    pars->addParam( new RichBool("normalize",false,"Normalize","Normalize to unitary bounding box"));
    // pars->addParam( new RichFloat("rotatedeg",.2,"Rotate cloud by..."));
}

void cloud_filter_example::applyFilter(RichParameterSet* pars){
    CloudModel* cloud = qobject_cast<CloudModel*>(model());
            
    /// Bounding box normalization
    bool normalize = pars->getBool("normalize");
    if(normalize){
        qDebug() << "Normalizing...";
        qDebug() << "Old bounding box: " << printBounding(cloud->getBoundingBox());
        QVector3D s = cloud->getBoundingBox().size();
        float scale = qMax(s.x(),qMax(s.y(),s.z()));
        QVector3D offset = cloud->getBoundingBox().center();
        
        for(int i=0; i<cloud->points.size(); i++){
            QVector4D& pt = cloud->points[i];
            pt.setX( (pt.x() - offset.x())/scale );
            pt.setY( (pt.y() - offset.y())/scale );
            pt.setZ( (pt.z() - offset.z())/scale );
        }
        cloud->updateBoundingBox();
        qDebug() << "New bounding box: " << printBounding(cloud->getBoundingBox());
    }       
        
#if 0
    /// Create rotation matrix
    QMatrix mat;
    mat.rotate(30);
    QMatrix3x3 mat3;
    mat3(0,0) = mat.m11();
    mat3(0,1) = mat.m12();
    mat3(1,0) = mat.m21();
    mat3(1,1) = mat.m22();
    
    foreach(QVector3D v, cloud->points())
        qDebug();
#endif   
    
    /// Reset the viewpoint
    drawArea()->resetViewport();
}

Q_EXPORT_PLUGIN(cloud_filter_example)