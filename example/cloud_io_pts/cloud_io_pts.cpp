#include "cloud_io_pts.h"
#include "CloudModel.h"

Model* cloud_io_pts::open(QString path){
    CloudModel* model = new CloudModel(path);
    
    FILE* in = fopen(qPrintable(path), "r");
    if (!in) throw StarlabException("Failed to open");

    char line[100];       
    float x,y,z;
    int err;

    /// Skip whites & #
    while( !feof(in) ){
		fgets(line, 100, in);
		if(strlen(line) > 4 && line[0] != '#')
			break;
	}

    /// Read data
    while( !feof(in) ){
        err = fscanf(in, "%f %f %f", &x,&y,&z);
        model->points.push_back( QVector3D(x,y,z) );
    }
    
    fclose(in);
    return model;    
}    
void cloud_io_pts::save(Model* model,QString path){
    CloudModel* cloud = safeCast(model);
    checkValidPath(path);
    FILE* fid = fopen( qPrintable(path), "w" );
    if( fid == NULL ) throw StarlabException("the file cannot be opened");
    
    foreach(QVector4D p, cloud->points){
        // fprintf( fid, "v %.10f %.10f %.10f\n", p[v].x(), p[v].y(), p[v].z() );        
    }
}

Q_EXPORT_PLUGIN(cloud_io_pts)
