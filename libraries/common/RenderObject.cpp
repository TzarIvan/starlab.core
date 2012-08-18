#include "RenderObject.h"
#include <QGLWidget>
#include <QDebug>

void RenderObject::Base::draw(){
    glColor3f(_color.redF(), _color.greenF(), _color.blueF());   
}

RenderObject::Triangle::Triangle(QVector3D p1, QVector3D p2, QVector3D p3, QColor color) : Base(0.0f,color){
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->_color = color;
}

void RenderObject::Triangle::draw(){
    Base::draw();

    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON); 
        glVertex3d(p1.x(),p1.y(),p1.z());
        glVertex3d(p2.x(),p2.y(),p2.z());
        glVertex3d(p3.x(),p3.y(),p3.z());
    glEnd();
}

RenderObject::Point::Point(QVector3D p, float size, QColor color) : Base(size,color){
    this->p = p;
}

void RenderObject::Point::draw(){
    Base::draw();
    
    // qDebug() << "RenderObject::Point::render()";
    glPointSize(_size);    
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
        glVertex3d(p.x(),p.y(),p.z());
    glEnd();
}

RenderObject::Segment::Segment(QVector3D p1, QVector3D p2, float size, QColor color):Base(size, color){
    this->p1 = p1;
    this->p2 = p2;
}

void RenderObject::Segment::draw(){
    Base::draw();

    glLineWidth(_size);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
        glVertex3d(p1.x(),p1.y(),p1.z());
        glVertex3d(p2.x(),p2.y(),p2.z());
    glEnd();
}

RenderObject::Ray::Ray(QVector3D orig, QVector3D dir, float size, QColor color, float scale):Base(size,color){
    this->orig=orig;
    this->dir=dir;
    this->_scale=scale;
}

void RenderObject::Ray::draw(){
    Base::draw();
    glLineWidth(_size);
    glDisable(GL_LIGHTING);
    QVector3D& p1=orig;
    QVector3D p2=orig+_scale*dir;
    glBegin(GL_LINES);
        glVertex3d(p1.x(),p1.y(),p1.z());
        glVertex3d(p2.x(),p2.y(),p2.z());
    glEnd();
}
