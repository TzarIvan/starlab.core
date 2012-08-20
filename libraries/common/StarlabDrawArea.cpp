#include <QGLWidget>
#include <QMouseEvent>

#include "StarlabDrawArea.h"
#include "Document.h"
#include "StarlabException.h"
#include "RenderObject.h"
#include "interfaces/ModePlugin.h"


QList<RenderPlugin*> StarlabDrawArea::renderers(){
    QList<RenderPlugin*> retval; ///< in order
    foreach(Model* model, document()->models()){
        Q_ASSERT(model!=NULL);
        RenderPlugin* renderer = _renderers.value(model,NULL);
        /// If a renderer didn't exist, instantiate a default and add        
        if(!renderer){
            /// Factory a new copy of the renderer
            QString pluginName = pluginManager()->getPreferredRenderer(model);
            renderer = pluginManager()->newRenderPlugin(pluginName,model);
            _renderers[model] = renderer;
        }        
        retval.append(renderer);
    }
    return _renderers.values(); 
}

RenderPlugin *StarlabDrawArea::activeRenderer(Model *model){
    RenderPlugin* plugin = _renderers.value(model,NULL);
    // if(plugin==NULL) throw StarlabException("No renderer defined for %s",qPrintable(model->name));
    return plugin;
}

void StarlabDrawArea::setRenderer(Model* model, QString pluginName){
    RenderPlugin* old = _renderers.value(model,NULL);
    if(pluginName==old->name())
        return;
    
    // qDebug("StarlabDrawArea::setRenderer(%s,%s)",qPrintable(model->name), qPrintable(pluginName));
    RenderPlugin* newPlugin = pluginManager()->newRenderPlugin(pluginName,model);
    
    /// Replace the plugins
    document()->pushBusy();
        if(old != NULL){
            old->finalize();
            old->deleteLater();
        }
        _renderers[model] = newPlugin;
    document()->popBusy();
    
    /// Refresh the window
    this->update();
}

void StarlabDrawArea::update(){
    // qDebug() << "StarlabDrawArea::update()";

    /// Don't update on a busy document
    if(document()->isBusy()) return;

    /// Update the metadata needed by the renderer
    /// e.g. stick data in vertex buffer.. etc..
    foreach(RenderPlugin* renderer, renderers())
        renderer->init();
    
    /// This will force a "paint" of the GL window
    updateGL();
}

StarlabDrawArea::StarlabDrawArea(StarlabMainWindow* mainWindow) 
    : _mainWindow(mainWindow){

    /// Determines which events are forwarded to Mode plugins
    installEventFilter(this);
    
    setGridIsDrawn(false);
    
    // Keyboard + Mouse behavior
    // setShortcut(SAVE_SCREENSHOT, Qt::CTRL + Qt::SHIFT + Qt::Key_S);
}

void StarlabDrawArea::resetViewport(){
    qglviewer::Vec center(0,0,0);
    QBox3D largestBBox;
    largestBBox.setExtents(QVector3D(-1,-1,-1),QVector3D(1,1,1));

    foreach(Model* m, document()->models()){
        const QBox3D bbox = m->getBoundingBox();
        if(bbox.intersects(largestBBox))
            largestBBox = bbox;
        /*Vector3 c = (m->bbox.minimum() + m->bbox.maximum()) * 0.5;
        center = qglviewer::Vec(c[0],c[1],c[2]);
        radius = (m->bbox.maximum() - m->bbox.minimum()).norm() * 0.5;*/
    }

    QVector3D extent(largestBBox.size());
    double radius = qMax(extent.x(), qMax(extent.y(), extent.z()));

    camera()->setSceneRadius(radius * 2);
    camera()->showEntireScene();
    camera()->setUpVector(qglviewer::Vec(0,0,1));
    camera()->setPosition(qglviewer::Vec(radius,radius,radius) + center);
    camera()->lookAt(center);
}

void StarlabDrawArea::init(){
    setBackgroundColor(QColor(255,255,255));   
    resetViewport();
}

void StarlabDrawArea::draw(){
    glEnable(GL_MULTISAMPLE); ///< Enables anti-aliasing

    /// Render each Model
    foreach(RenderPlugin* renderer, renderers()){
        glPushMatrix();
            glMultMatrixd( document()->transform.data() );
            renderer->render();
        glPopMatrix();        
    }   

    /// @todo Render decoration plugins
    
    /// Render mode decoration
    if( mainWindow()->hasActiveMode() )
        mainWindow()->activeMode()->decorate();       
}

void StarlabDrawArea::drawWithNames()
{
}

StarlabDrawArea::~StarlabDrawArea(){
    // qDebug() << "StarlabDrawArea::~StarlabDrawArea()";
    deleteAllRenderObjects();
}

void StarlabDrawArea::deleteAllRenderObjects(){
    document()->pushBusy();
    foreach(RenderObject::Base* obj, renderObjectList)
        delete obj;
    renderObjectList.clear();
    document()->popBusy();
}

void StarlabDrawArea::drawAllRenderObjects(){
    foreach(RenderObject::Base* obj, renderObjectList)
        obj->draw();
}

RenderObject::Triangle& StarlabDrawArea::drawTriangle(QVector3D p1, QVector3D p2, QVector3D p3, QColor color){
    RenderObject::Triangle* triangle = new RenderObject::Triangle(p1,p2,p3,color);
    renderObjectList.append(triangle);
    return *triangle;
}

RenderObject::Point& StarlabDrawArea::drawPoint(QVector3D p1, float size, QColor color){
    RenderObject::Point* point = new RenderObject::Point(p1,size,color);
    renderObjectList.append(point);
    return *point;
}

RenderObject::Segment& StarlabDrawArea::drawSegment(QVector3D p1, QVector3D p2, float size, QColor color){
    RenderObject::Segment* edge = new RenderObject::Segment(p1,p2,size,color);
    renderObjectList.append(edge);
    return *edge;
}

RenderObject::Ray& StarlabDrawArea::drawRay(QVector3D orig, QVector3D dir, float size, QColor color, float scale){
    RenderObject::Ray* edge = new RenderObject::Ray(orig,dir,size,color,scale);
    renderObjectList.append(edge);
    return *edge;
}

/// @internal returning true will prevent the drawArea plugin from intercepting the events!!!
bool StarlabDrawArea::eventFilter(QObject*, QEvent* event){
    /// If a mode is not open, pass *everything* to the drawArea plugin
    if(!mainWindow()->hasActiveMode()) return false;

    /// If it is open, pass it to the handlers
    ModePlugin* mode = mainWindow()->activeMode();
    switch(event->type()){
        case QEvent::MouseButtonRelease: return mode->mouseReleaseEvent((QMouseEvent*)event); break;
        case QEvent::MouseButtonPress:   return mode->mousePressEvent((QMouseEvent*)event); break;
        case QEvent::MouseMove:          return mode->mouseMoveEvent((QMouseEvent*)event); break;
        case QEvent::KeyPress:           return mode->keyPressEvent((QKeyEvent*)event); break;
        case QEvent::Wheel:              return mode->wheelEvent((QWheelEvent*)event); break;
        default: return true; ///< Any other event is blocked!!
    }  
}

void StarlabDrawArea::deleteRenderObject(RenderObject* /*object*/){
    /// @todo 
    throw StarlabException("TODO: StarlabDrawArea::deleteRenderObject");
}
