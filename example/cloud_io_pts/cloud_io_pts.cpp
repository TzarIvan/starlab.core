#include "cloud_io_pts.h"
#include "CloudModel.h"

typedef CloudModel::Point Point;

Model* cloud_io_pts::open(QString path){
    CloudModel* model = new CloudModel(path);
    
    FILE* in = fopen(qPrintable(path), "r");
    if (!in) throw StarlabException("Failed to open");

    char line[100];       
    float x,y,z;

    /// Skip whites & #
    while( !feof(in) ){
        if( fgets(line, 100, in) )
            if(strlen(line) > 4 && line[0] != '#')
                break;
    }
    /// Read data
    while( !feof(in) ){
        if( fscanf(in, "%f %f %f", &x,&y,&z)==3 )
            model->points.push_back( new Point(x,y,z) );
    }
    
    fclose(in);
    return model;    
}    
void cloud_io_pts::save(Model* model,QString path){
    CloudModel* cloud = safeCast(model);
    FILE* fid = fopen( qPrintable(path), "w" );
    if(fid==NULL) throw StarlabException("the file cannot be opened");
    
    foreach(Point* point, cloud->points){
        QVector3D& p = point->coord; 
        fprintf(fid, "%f %f %f\n", p.x(),p.y(),p.z());
    }
}

Q_EXPORT_PLUGIN(cloud_io_pts)
