#include "cloud_render_points.h"
#include <qgl.h>

void cloud_render_points::render(){
    glColor3f(model()->color.redF(),model()->color.greenF(),model()->color.blueF());

    glDisable(GL_LIGHTING);
    glPointSize(3);
    glBegin(GL_POINTS);
        foreach(QVector4D v, cloud()->points)
            glVertex3d(v.x(), v.y(), v.z());
    glEnd();
    glEnable(GL_LIGHTING);
}

Q_EXPORT_PLUGIN(cloud_render_points)
