#include "StarlabDrawArea.h"
#include "Document.h"
#include <QGLWidget>
#include "StarlabException.h"
#include "RenderObject.h"

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
    widget()->update();
}

StarlabDrawArea::StarlabDrawArea(StarlabApplication* app) 
    : _application(app){}

StarlabDrawArea::~StarlabDrawArea(){
    // qDebug() << "StarlabDrawArea::~StarlabDrawArea()";
    deleteAllRenderObjects();
    if(widget()!=NULL)
        delete widget();
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

void StarlabDrawArea::deleteRenderObject(RenderObject* /*object*/){
    /// @todo 
    throw StarlabException("invalid");
}
