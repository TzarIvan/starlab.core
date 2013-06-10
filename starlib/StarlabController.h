#pragma once
#include <QObject>
#include <QMouseEvent>
#include "StarlabDrawArea.h"
#include "ModePlugin.h"

namespace Starlab{

class Controller : public QObject{
    Q_OBJECT
    friend class ControllerManager;
    
/// @{ access to graphics context
protected:
    StarlabDrawArea* _drawArea;
public:
    StarlabDrawArea* drawArea(){ return _drawArea; }    
/// @}
    
/// @{ position control
private:    
    /// General 3D position of the controller
    Vector3 _position;
public:
    const Vector3& center(){ 
        return _position; 
    }
    void setPosition(Vector3 position){
        _position = position;
        emit positionUpdated(_position);
    }
signals:
    void positionUpdated(Vector3 position);
/// @} 

/// @{ subcomponent selection    
public slots:
    void setSelectedSubcomponent(int id){ selected_subcomponent = id; }
signals:
    void selectedSubcomponentChanged(int id);
protected:
    /// Set by ....
    int selected_subcomponent;
/// @}    


protected:
    Controller(Vector3 center){
        this->_position = center;
        this->selected_subcomponent = -1;
    }

public:
    virtual void draw() = 0;
    virtual void drawWithNames() = 0;

public:
    virtual void mousePressEvent(QMouseEvent* event){ Q_UNUSED(event) }
    virtual void mouseReleaseEvent(QMouseEvent* event){ Q_UNUSED(event) }
    virtual void mouseMoveEvent(QMouseEvent* event){ Q_UNUSED(event) }    
};


class ControllerManager{
protected:
    friend class Controller;
    /// A list of controllers
    QList<Controller*> _list;
    /// Might be null when nothing is selected
    Controller* _selected;
    /// Root (to fetch drawArea)
    ModePlugin* plugin;
    /// Fetches qglViewer context   
    DrawArea* drawArea(){ return plugin->drawArea(); }
   
public:
    ControllerManager(ModePlugin* plugin){
        this->plugin = plugin;
        this->_selected = NULL;
    }
    
    void clear(){
        this->_selected = NULL;
        foreach(Controller* c, _list)
            delete c;
        _list.clear();
    }
    
    void draw(){   
        glEnable(GL_LIGHTING);
        foreach(Controller* c, _list)
            c->draw();
    }
    
    void drawWithNames(){
        // qDebug() << "ControllerModePlugin::drawWithNames";
        glInitNames();
        
        /// Draw the scene with invalid name, so that invisible controllers are not selectable
        glPushName(-1);
            drawArea()->draw_models();
        glPopName();
        
        // qDebug() << "ControllerManager::draw_with_names()";
        /// Clears the name buffer
        if(_selected==NULL){
            // qDebug() << "drawing main component names";
            int i=0;
            foreach(Controller* c, _list){
                glPushName(i++);
                    c->draw();                    
                glPopName();
            }
        } else {
            // qDebug() << "drawing sub component names";
            _selected->drawWithNames();            
        }
    }
    
    ControllerManager& operator<<(Controller *controller){
        this->add( controller );
        return *this;
    }
    
    void add(Controller *controller){
        _list.push_back(controller);
        controller->_drawArea = drawArea();
    }
    
    bool mousePressEvent(QMouseEvent *event){
        QPoint pickpoint = event->pos();
        
        /// Fetch the controller
        _selected = NULL;
        drawArea()->select(pickpoint); 
        /// a call to "draw_with_names()" is made here
        int offset = drawArea()->selectedName();
        // qDebug() << "Selected controller #:" << offset;        
        
        if( offset>=0 && offset<_list.size() ){
            _selected = _list.at(offset);
        }
                
        /// Attempt identify sub-controller
        if(_selected!=NULL){
            drawArea()->select(pickpoint);
            /// a call to "draw_with_names()" is made here
            int offset = drawArea()->selectedName();
            // qDebug() << "Sub-controller #:" << offset;
            _selected->setSelectedSubcomponent(offset);
        } else
            return false;
     
        /// Pass even to the controller itself
        _selected->mousePressEvent(event);
        return true;
    }
    
    bool mouseReleaseEvent(QMouseEvent *event){
        // qDebug() << "ControllerModePlugin::mouseReleaseEvent";
        if(_selected == NULL)
            return false;
    
        /// @todo check! treat is as a filter
        _selected->mouseReleaseEvent(event);
        return true;    
    }
    
    bool mouseMoveEvent(QMouseEvent *event){
        if(_selected == NULL)
            return false;
    
        /// @todo check! treat is as a filter
        _selected->mouseMoveEvent(event);   
        return true;    
    }
};


// picking with LINE/POINT does not work well :( :(
// glBegin(GL_LINES); glVertex3d(0,0,0); glVertex3d(1.0,0,0); glEnd();            
class FrameController : public Controller{
    Q_OBJECT
    friend class ControllerModePlugin;
    
    /// Numeric ID of portions used for picking
    enum NAMES {SPHERE,XAXIS,YAXIS,ZAXIS};
    
    /// Reference frame 
    Eigen::Matrix3d frame;

    /// Metadata for controller
    Plane3  pan_hyperplane;
    Vector3 click_origin;
    Vector3 center_before;
    
private:
    inline Ray3 x_ray(){ return Ray3(center_before,frame.col(0)); }
    inline Ray3 y_ray(){ return Ray3(center_before,frame.col(1)); }
    inline Ray3 z_ray(){ return Ray3(center_before,frame.col(2)); }
    inline Plane3 x_dir_space(Vector3 dir){ return Plane3(dir-(dir.dot(frame.col(0)))*frame.col(0),center_before ); } 
    inline Plane3 y_dir_space(Vector3 dir){ return Plane3(dir-(dir.dot(frame.col(1)))*frame.col(1),center_before ); } 
    inline Plane3 z_dir_space(Vector3 dir){ return Plane3(dir-(dir.dot(frame.col(2)))*frame.col(2),center_before ); } 
    
public:   
    FrameController(Vector3 center):Controller(center){
        /// Init reference frame
        frame.setIdentity(3,3);   
        /// This will be set when it's added to the master controller
        this->_drawArea = NULL;
    }   
    
private:
    void draw_arrow(){
        static GLUquadric* quadric = gluNewQuadric();        
        
        /// Params
        int cylinder_num_segments = 10;
        double cylinder_size = .04;
        double cylinder_len = 1.0;        
        
        /// Body
        gluCylinder(quadric, cylinder_size, cylinder_size, cylinder_len, cylinder_num_segments, 1);
        
        /// Tip
        glTranslated(0.0,0.0,1.0);
        gluCylinder(quadric,.07, .00, .12, cylinder_num_segments, 1);
    }

    void draw_sphere(bool withname=false){
        static GLUquadric* quadric = gluNewQuadric();        
        if(withname) glPushName(SPHERE);
            glColor3f(.9,.9,.9);
            gluSphere(quadric, 0.08, 10, 10);
        if(withname) glPopName();
    }
    void draw_xaxis(bool withname=false){
        if(withname) glPushName(XAXIS);
            glPushMatrix();
                glColor3f(1.0,.0,0.0);
                glRotated(90,0,1,0); 
                draw_arrow();
            glPopMatrix();
        if(withname) glPopName();        
    }
    void draw_yaxis(bool withname=false){
        if(withname) glPushName(YAXIS);
            glPushMatrix();
                glColor3f(0.0,1.0,0.0);
                glRotated(90,1,0,0); 
                draw_arrow();
            glPopMatrix();
        if(withname) glPopName(); 
    }
    void draw_zaxis(bool withname=false){
        if(withname) glPushName(ZAXIS);
            glPushMatrix();
                glColor3f(0.0,0.0,1.0);
                draw_arrow();
            glPopMatrix();
        if(withname) glPopName();
    }
    
    void _draw(bool withnames){
        glColor3f(0.8,0.8,0.8);
        glPushMatrix();
            glTranslated(center()[0], center()[1], center()[2]);
            draw_sphere(withnames);
            draw_xaxis(withnames);
            draw_yaxis(withnames);
            draw_zaxis(withnames);
        glPopMatrix();
    }
    
public:
    void draw(){ 
        return _draw(false); 
    }
    void drawWithNames(){ 
        //qDebug() << "Controller::drawWithNames()";
        return _draw(true); 
    }

public:
    void mousePressEvent(QMouseEvent* event){
        // qDebug() << "mousePressEvent";
        /// When clicking on sphere
        QPoint p = event->pos();
        Ray3 ray = drawArea()->convert_click_to_ray(p);
        
        /// cache original position
        center_before = center();

        switch(selected_subcomponent){
        case XAXIS:
            click_origin = ray.intersectionPoint(x_dir_space(ray.direction()));
            click_origin = x_ray().projection(click_origin);
            break;
        case YAXIS:
            click_origin = ray.intersectionPoint(y_dir_space(ray.direction()));
            click_origin = y_ray().projection(click_origin);
            break;
        case ZAXIS:
            click_origin = ray.intersectionPoint(z_dir_space(ray.direction()));
            click_origin = z_ray().projection(click_origin);
            break;
        case SPHERE:
        default: 
            /// Cache transformation space (+Z plane w.r.t. the camera)
            pan_hyperplane = Plane3(ray.direction(), center());
            click_origin = ray.intersectionPoint(pan_hyperplane);
        }
        
        drawArea()->updateGL();
    }
    
    void mouseReleaseEvent(QMouseEvent* /*event*/){ }    
    
    void mouseMoveEvent(QMouseEvent* event){
        // qDebug() << "mouseMoveEvent";        
        QPoint p = event->pos();
        Ray3 ray = drawArea()->convert_click_to_ray(p);
        
        /// C++ does not allow declarations inside switch
        Vector3 planar_isec, proj, offset;
        
        /// How should we change the frame?
        switch(selected_subcomponent){
        case XAXIS: 
            planar_isec = ray.intersectionPoint(x_dir_space(ray.direction()));
            proj = x_ray().projection(planar_isec);
            break;
        case YAXIS:
            planar_isec = ray.intersectionPoint(y_dir_space(ray.direction()));
            proj = y_ray().projection(planar_isec);
            break;
        case ZAXIS:
            planar_isec = ray.intersectionPoint(z_dir_space(ray.direction()));
            proj = z_ray().projection(planar_isec);
            break;            
        case SPHERE:
        default:
            proj = ray.intersectionPoint(pan_hyperplane);        
            break;
        }

        /// Update position
        offset = proj - click_origin;
        setPosition(center_before + offset);
        
        /// Update the visualization
        drawArea()->updateGL();
    }
};

} /// namespace

typedef Starlab::Controller StarlabController;
