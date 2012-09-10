#include <QGLWidget>
#include <QMouseEvent>

#include "StarlabDrawArea.h"
#include "Document.h"
#include "StarlabException.h"
#include "RenderObject.h"
#include "interfaces/ModePlugin.h"

using namespace qglviewer;

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
    // qDebug("StarlabDrawArea::setRenderer(%s,%s)",qPrintable(model->name), qPrintable(pluginName));
    document()->pushBusy();
        removeRenderer(model);
        _renderers[model] = pluginManager()->newRenderPlugin(pluginName,model);
    document()->popBusy();
}

void StarlabDrawArea::removeRenderer(Model* model){
    // qDebug() << "StarlabDrawArea::removeRenderer()";
    document()->pushBusy();
        RenderPlugin* rend = _renderers.value(model,NULL);
        if(rend==NULL) throw StarlabException("FATAL: StarlabDrawArea::removeRenderer()");
        _renderers.remove(model);
        rend->finalize();
        rend->deleteLater();
    document()->popBusy();
}

void StarlabDrawArea::update(){
    // qDebug() << "StarlabDrawArea::update()";
    
    /// @internal Initialization can act on busy document
    /// Update the metadata needed by the renderer
    /// e.g. stick data in vertex buffer.. etc..
    foreach(RenderPlugin* renderer, renderers())
        renderer->init();
    
    /// Don't update on a busy document
    if(document()->isBusy()) return;

    /// This will force a "paint" of the GL window
    updateGL();
}

StarlabDrawArea::StarlabDrawArea(StarlabMainWindow* mainWindow) 
    : _mainWindow(mainWindow){

    /// When document changes, refresh the rendering
    connect(document(), SIGNAL(hasChanged()), this, SLOT(update()));
    /// When a model is deleted, tell the renderer to stop working on it
    connect(document(), SIGNAL(deleteScheduled(Model*)), this, SLOT(removeRenderer(Model*)));
    /// Determines which events are forwarded to Mode plugins
    installEventFilter(this);
    
    /// @todo restore trackball
    // settings()->setDefault("StarlabDrawArea/showtrackball",false);

    /// Default camera
    camera()->setType(Camera::ORTHOGRAPHIC);
    
    /// @todo setup View->ToggleFPS
    /// @todo why is update broken when nothing is moving?
    this->setFPSIsDisplayed(true);

    // setGridIsDrawn(false);
    
    /// @todo Add screenshot to View->Take Screenshot
    // Keyboard + Mouse behavior
    // setShortcut(SAVE_SCREENSHOT, Qt::CTRL + Qt::SHIFT + Qt::Key_S);
}

void StarlabDrawArea::getSceneBounds(QVector3D &minbound, QVector3D &maxbound)
{
    foreach(Model* m, document()->models()){
        QBox3D bbox = m->bbox();
        minbound = QVector3D(qMin(bbox.minimum().x(),minbound.x()),
                             qMin(bbox.minimum().y(),minbound.y()),
                             qMin(bbox.minimum().z(),minbound.z()));

        maxbound = QVector3D(qMax(bbox.maximum().x(),maxbound.x()),
                             qMax(bbox.maximum().y(),maxbound.y()),
                             qMax(bbox.maximum().z(),maxbound.z()));
    }
}

void StarlabDrawArea::resetViewport(){
    QVector3D minbound(-1,-1,-1);
    QVector3D maxbound( 1, 1, 1);

    if(document()->models().size()){
        double l = 1e20;
        minbound = QVector3D( l, l, l);
        maxbound = QVector3D(-l,-l,-l);
    }

    getSceneBounds(minbound, maxbound);

    Vec min_bound(minbound.x(),minbound.y(),minbound.z());
    Vec max_bound(maxbound.x(),maxbound.y(),maxbound.z());

    camera()->fitBoundingBox( min_bound, max_bound );
    camera()->setSceneRadius((max_bound - min_bound).norm()*0.6);
    camera()->setSceneCenter((min_bound + max_bound) * 0.5);
    camera()->showEntireScene();
}

void StarlabDrawArea::setPerspectiveProjection(){
    camera()->setType(Camera::PERSPECTIVE); updateGL();
}

void StarlabDrawArea::setOrthoProjection(){
    camera()->setType(Camera::ORTHOGRAPHIC); updateGL();
}

void StarlabDrawArea::setIsoProjection(){
    setOrthoProjection();

    // Move camera such that entire scene is visisble
    double r = camera()->sceneRadius();
    Frame f(camera()->sceneCenter() + Vec(r,-r,r), Quaternion());
    f.rotate(Quaternion(Vec(0,0,1), M_PI / 4.0));
    f.rotate(Quaternion(Vec(1,0,0), M_PI / 3.3));
    camera()->interpolateTo(f,0.25);
}

void StarlabDrawArea::viewFrom(QAction * a){
    Frame f;

    QStringList list;
    list << "Top" << "Bottom" << "Left" << "Right" << "Front" << "Back";

    QBox3D bbox = document()->selectedModel()->bbox();

    double e = bbox.size().length() * 2;
    Vec c(bbox.center().x(),bbox.center().y(),bbox.center().z());

    switch(list.indexOf(a->text()))
    {
    case 0:
        f = Frame(c+Vec(0,0,e), Quaternion());
        break;
    case 1:
        f = Frame(c+Vec(0,0,-e), Quaternion());
        f.rotate(Quaternion(Vec(1,0,0), M_PI));
        break;
    case 2:
        f = Frame(c+Vec(0,-e,0), Quaternion(Vec(0,0,1),Vec(0,-1,0)));
        break;
    case 3:
        f = Frame(c+Vec(0,e,0), Quaternion(Vec(0,0,1),Vec(0,1,0)));
        f.rotate(Quaternion(Vec(0,0,1), M_PI));
        break;
    case 4:
        f = Frame(c+Vec(e,0,0), Quaternion(Vec(0,0,1),Vec(1,0,0)));
        f.rotate(Quaternion(Vec(0,0,1), M_PI / 2.0));
        break;
    case 5:
        f = Frame(c+Vec(-e,0,0), Quaternion(Vec(0,0,-1),Vec(1,0,0)));
        f.rotate(Quaternion(Vec(0,0,-1), M_PI / 2.0));
        break;
    }
    camera()->interpolateTo(f,0.25);

    camera()->setSceneCenter(c);
}

void StarlabDrawArea::init(){
    /// Background color from settings file
    QString key = "DefaultBackgroundColor";
    settings()->setDefault( key, QVariant(QColor(50,50,60)) );
    setBackgroundColor( settings()->getQColor(key) );

    camera()->setUpVector(Vec(0,0,1));

    resetViewport();
}

void StarlabDrawArea::draw(){
    glEnable(GL_MULTISAMPLE); ///< Enables anti-aliasing

    /// Render each Model
    foreach(RenderPlugin* renderer, renderers()){
        glPushMatrix();
            glMultMatrixd( document()->transform.data() );
            if(renderer->model()->isVisible) renderer->render();
        glPopMatrix();        
    }   

    /// @todo Render decoration plugins

    /// Render renderable objects
    drawAllRenderObjects();

    /// Render mode decoration
    if( mainWindow()->hasActiveMode() )
        mainWindow()->activeMode()->decorate();
}

void StarlabDrawArea::drawWithNames(){
    if(mainWindow()->hasActiveMode())
        mainWindow()->activeMode()->drawWithNames();
}

void StarlabDrawArea::endSelection(const QPoint & p)
{
    if(mainWindow()->hasActiveMode())
        mainWindow()->activeMode()->endSelection(p);
	else
		QGLViewer::endSelection(p);
}

void StarlabDrawArea::postSelection(const QPoint & p)
{
    if(mainWindow()->hasActiveMode())
        mainWindow()->activeMode()->postSelection(p);
}

StarlabDrawArea::~StarlabDrawArea(){
    // qDebug() << "StarlabDrawArea::~StarlabDrawArea()";
    deleteAllRenderObjects();
}

void StarlabDrawArea::deleteAllRenderObjects(){
    document()->pushBusy();

    qDeleteAll(renderObjectList.begin(), renderObjectList.end());
    renderObjectList.clear();

    document()->popBusy();
}

void StarlabDrawArea::drawAllRenderObjects(){
    foreach(RenderObject::Base* obj, renderObjectList)
        obj->draw();
}

void StarlabDrawArea::addRenderObject(RenderObject::Base * obj)
{
    renderObjectList.append(obj);
}

RenderObject::Triangle& StarlabDrawArea::drawTriangle(QVector3D p1, QVector3D p2, QVector3D p3, QColor color){
    RenderObject::Triangle* triangle = new RenderObject::Triangle(p1,p2,p3,color);
    addRenderObject(triangle);
    return *triangle;
}

RenderObject::Point& StarlabDrawArea::drawPoint(QVector3D p1, float size, QColor color){
    RenderObject::Point* point = new RenderObject::Point(p1,size,color);
    addRenderObject(point);
    return *point;
}

RenderObject::Segment& StarlabDrawArea::drawSegment(QVector3D p1, QVector3D p2, float size, QColor color){
    RenderObject::Segment* segment = new RenderObject::Segment(p1,p2,size,color);
    addRenderObject(segment);
    return *segment;
}

RenderObject::Ray& StarlabDrawArea::drawRay(QVector3D orig, QVector3D dir, float size, QColor color, float scale){
    RenderObject::Ray* ray = new RenderObject::Ray(orig,dir,size,color,scale);
    addRenderObject(ray);
    return *ray;
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
        default: return false;
    }  
}

void StarlabDrawArea::deleteRenderObject(RenderObject* /*object*/){
    /// @todo 
    throw StarlabException("TODO: StarlabDrawArea::deleteRenderObject");
}
