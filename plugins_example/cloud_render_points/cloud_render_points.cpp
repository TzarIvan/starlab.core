#include "cloud_render_points.h"
#include <qgl.h>

void cloud_render_points::render(){
    CloudModel* cloud = qobject_cast<CloudModel*>(model());
    QColor& c = model()->color;
    glColor3f(c.redF(),c.greenF(),c.blueF());
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
        foreach(QVector4D v, cloud->points)
            glVertex3d(v.x(), v.y(), v.z());
    glEnd();
    glEnable(GL_LIGHTING);
}

Q_EXPORT_PLUGIN(cloud_render_points)
