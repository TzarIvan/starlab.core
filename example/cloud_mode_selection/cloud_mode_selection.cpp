#include <QMouseEvent>
#include "cloud_mode_selection.h"
#include "StarlabDrawArea.h"
#include <qgl.h>

bool cloud_mode_selection::mousePressEvent(QMouseEvent* event){
    // qDebug() << "cloud_mode_selection::mousePressEvent()";
    start=event->pos();
    cur=start;    
    return true;
}

bool cloud_mode_selection::mouseMoveEvent(QMouseEvent* event){
    // qDebug() << "cloud_mode_selection::mouseMoveEvent()";
    /// @todo update the selection at runtime
    prev=cur;
    cur=event->pos();
    isDragging = true;
    drawArea()->updateGL();
    return true;
}

bool cloud_mode_selection::mouseReleaseEvent(QMouseEvent *){
    drawArea()->updateGL();
    return true;
}

void cloud_mode_selection::decorate(){
    // qDebug() << "cloud_mode_selection::paintEvent";
    drawSelectionRectangle();
}

void cloud_mode_selection::drawWithNames(){
    qDebug() << "cloud_mode_selection::drawWithNames()";
    glPushMatrix();
        glMultMatrixd( document()->transform.data() );
        foreach(QVector4D v, cloud()->points)
            glVertex3d(v.x(), v.y(), v.z());
    glPopMatrix();     
}

void cloud_mode_selection::drawSelectionRectangle(){  
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        int width = drawArea()->width();
        int height = drawArea()->height();
        glOrtho(0,width,height,0,-1,1);
            
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4d(1.0, 0.0, 0.0, 0.2);
        glBegin(GL_LINE_STRIP);
            glVertex2f(start.x(),start.y());
            glVertex2f(cur.x(),start.y());
            glVertex2f(cur.x(),cur.y());
            glVertex2f(start.x(),cur.y());
            glVertex2f(start.x(),start.y());
        glEnd();
    glPopAttrib();
}

Q_EXPORT_PLUGIN(cloud_mode_selection)
