#include <QMouseEvent>
#include "cloud_mode_selection.h"
#include "StarlabDrawArea.h"
#include <qgl.h>
#include "glu.h"

bool cloud_mode_selection::mousePressEvent(QMouseEvent* event){
    // qDebug() << "cloud_mode_selection::mousePressEvent()";
    start=event->pos();
    cur=start;    
    return true;
}

bool cloud_mode_selection::mouseMoveEvent(QMouseEvent* event){
    // qDebug() << "cloud_mode_selection::mouseMoveEvent()";
    prev=cur;
    cur=event->pos();
    isDragging = true;
    drawArea()->widget()->update();
    return true;
}

bool cloud_mode_selection::paintEvent(QPaintEvent *){
    qDebug() << "cloud_mode_selection::paintEvent";
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSelectionRectangle();
    drawArea()->widget()->swapBuffers();
    return true;
}

void cloud_mode_selection::drawSelectionRectangle(){  
    qDebug() << "cloud_mode_selection::drawTest()";
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int width = drawArea()->widget()->width();
    int height = drawArea()->widget()->height();
    glOrtho(0,width,height,0,-1,1);
        
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS); 
        glBegin(GL_LINE_LOOP);
            glVertex2f(start.x(),start.y());
            glVertex2f(cur.x(),start.y());
            glVertex2f(cur.x(),cur.y());
            glVertex2f(start.x(),cur.y());
        glEnd();
    glEnd();   
}


Q_EXPORT_PLUGIN(cloud_mode_selection)
