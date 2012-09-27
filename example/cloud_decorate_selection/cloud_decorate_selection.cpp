#include "cloud_decorate_selection.h"
#include <QGLWidget>
#include "StarlabDrawArea.h"

void cloud_decorate_selection::decorate(){
    // Draw selected items
    drawArea()->qglColor(QColor(255,0,0));
    glPointSize(10);
    
    QVector<QVariant> vselect = cloud()->getProperty("selected");
    if(vselect.isEmpty()) return;
    
    glDisable(GL_LIGHTING);
        glBegin(GL_POINTS);
        for(int i = 0; i < vselect.size(); i++){
            if(vselect[i] == true){
                QVector3D v = cloud()->points[i]->coord;
                glVertex3d(v.x(), v.y(), v.z());
            }
        }
        glEnd();
    glEnable(GL_LIGHTING);
}

Q_EXPORT_PLUGIN(cloud_decorate_selection)
