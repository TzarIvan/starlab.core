#pragma once
#include <QColor>
#include <QVector3D>

class RenderObject{
public:
    
    class Base{
    protected:
        float _size;
        QColor _color;
    public:
        Base(float size,QColor color) : _size(size),_color(color){}
        virtual ~Base(){}
        virtual void draw()=0;
        Base& size(float size){ this->_size=size; return *this; }
        Base& color(QColor color){ this->_color=color; return *this; }
    };
    
    class Triangle : public Base{
    public:
        QVector3D p1,p2,p3;
        Triangle(QVector3D p1, QVector3D p2, QVector3D p3, QColor color=Qt::red);
        virtual void draw();
    };

    class Segment : public Base{
    public:
        QVector3D p1,p2;
        Segment(QVector3D p1, QVector3D p2, float size, QColor color=Qt::red);
        virtual void draw();       
    };

    class Ray : public Base{
    public:
        QVector3D orig;
        QVector3D dir;
        float _scale;
        
        Ray(QVector3D orig, QVector3D dir, float size, QColor color=Qt::red, float scale=1);
        virtual void draw();       
        Ray& scale(float scale){ this->_scale=scale; return *this; }
        
    };
    
    class Point : public Base{
    public:
        QVector3D p;
        Point(QVector3D p, float size, QColor color=Qt::red);
        virtual void draw();
    };
};
