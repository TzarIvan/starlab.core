#include "plugin.h"
#include <QtOpenGL>

class DotCloudRenderer : public CloudRenderer{
    void render(){
        glColor3f(model()->color.redF(),model()->color.greenF(),model()->color.blueF());
    
        glDisable(GL_LIGHTING);
        glPointSize(3);
        glBegin(GL_POINTS);
            foreach(CloudModel::Point* p, cloud()->points){
                QVector3D& v=p->coord;
                glVertex3d(v.x(), v.y(), v.z());
            }
        glEnd();
        glEnable(GL_LIGHTING);
    }
};

Renderer* plugin::instance(){ return new DotCloudRenderer(); }
Q_EXPORT_PLUGIN(plugin)
