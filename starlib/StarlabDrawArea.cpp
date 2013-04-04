#include <QGLWidget>
#include <QMouseEvent>
#include <QColorDialog>

#include "StarlabDrawArea.h"
#include "Document.h"
#include "StarlabException.h"
#include "RenderObject.h"
#include "ModePlugin.h"
#include "DecoratePlugin.h"

using namespace Starlab;
using namespace qglviewer;

/// By default the static instance is NULL
DrawArea* DrawArea::_staticInstance = NULL;

void DrawArea::update(){
    // qDebug() << "StarlabDrawArea::update()";
    
    /// @internal Initialization can act on busy document
    /// Update the metadata needed by the renderer
    /// e.g. stick data in vertex buffer.. etc..
    foreach(Model* model, document()->models()){
        /// Create instance if renderer missing
        if(model->renderer()==NULL){
            QString name = pluginManager()->getPreferredRenderer(model);
            RenderPlugin* plugin = pluginManager()->getRenderPlugin(name);
            model->setRenderer( plugin );
        } else {
            /// just re-initialize it
            model->renderer()->init();
        }
    }
    
    /// Don't update on a busy document
    if(document()->isBusy()) 
        return;

    /// This will force a "paint" of the GL window
    updateGL();
}

DrawArea::DrawArea(MainWindow* mainWindow) 
    : _mainWindow(mainWindow){

    /// When document changes, refresh the rendering
    connect(document(), SIGNAL(hasChanged()), this, SLOT(update()));
    /// Determines whether events are forwarded to Mode plugins
    installEventFilter(this);
    
    /// @todo restore trackball
    // settings()->setDefault("StarlabDrawArea/showtrackball",false);

    /// Default camera
    camera()->setType(Camera::ORTHOGRAPHIC);
    
    /// Value of 100.0 forbids spinning
    camera()->frame()->setSpinningSensitivity(100);

    /// Disable saving state to file
    setStateFileName( QString::null );
    
    /// @todo setup View->ToggleFPS
    /// @todo why is update broken when nothing is moving?
    this->setFPSIsDisplayed(true);

    // setGridIsDrawn(false);
    
    /// @todo Add screenshot to View->Take Screenshot
    // Keyboard + Mouse behavior
    // setShortcut(SAVE_SCREENSHOT, Qt::CTRL + Qt::SHIFT + Qt::Key_S);

    this->captureDepthBuffer = false;
    
    
    /// Disables all default qglviewer predefined keyboard shortcuts
    {
        setShortcut(DRAW_AXIS,0);
        setShortcut(DRAW_GRID,0);
        setShortcut(DISPLAY_FPS,0);
        setShortcut(STEREO,0);
        setShortcut(HELP,0);
        setShortcut(EXIT_VIEWER,0);
        setShortcut(CAMERA_MODE,0);
        setShortcut(ANIMATION,0);
        setShortcut(SAVE_SCREENSHOT,0);
        setShortcut(FULL_SCREEN,0);
        setShortcut(EDIT_CAMERA,0);
        setShortcut(INCREASE_FLYSPEED,0);
        setShortcut(DECREASE_FLYSPEED,0);
        setShortcut(MOVE_CAMERA_LEFT,0);
        setShortcut(MOVE_CAMERA_RIGHT,0);
        setShortcut(MOVE_CAMERA_UP,0);
        setShortcut(MOVE_CAMERA_DOWN,0);
    }
    
    /// Saves a static instance
    Q_ASSERT(DrawArea::_staticInstance==NULL);
    DrawArea::_staticInstance = this;
}

void DrawArea::resetViewport(){

    QBox3D sceneBBox = document()->bbox();
    QVector3D minbound = sceneBBox.minimum();
    QVector3D maxbound = sceneBBox.maximum();

    Vec min_bound(minbound.x(),minbound.y(),minbound.z());
    Vec max_bound(maxbound.x(),maxbound.y(),maxbound.z());

    camera()->fitBoundingBox( min_bound, max_bound );
    camera()->setSceneRadius((max_bound - min_bound).norm() * 0.4);
    camera()->setSceneCenter((min_bound + max_bound) * 0.5);
    camera()->showEntireScene();
}

void DrawArea::setPerspectiveProjection(){
    camera()->setType(Camera::PERSPECTIVE); updateGL();
}

void DrawArea::setOrthoProjection(){
    camera()->setType(Camera::ORTHOGRAPHIC); updateGL();
}

void DrawArea::setIsoProjection(){
    setOrthoProjection();

    // Move camera such that entire scene is visisble
    double r = camera()->sceneRadius();
    Frame f(camera()->sceneCenter() + Vec(r,-r,r), Quaternion());
    f.rotate(Quaternion(Vec(0,0,1), M_PI / 4.0));
    f.rotate(Quaternion(Vec(1,0,0), M_PI / 3.3));
    camera()->interpolateTo(f,0.25);
}

void DrawArea::viewFrom(QAction * a){

    if(!document()->selectedModel()) return;

    Frame f;

    QStringList list;
    list << "Top" << "Bottom" << "Left" << "Right" << "Front" << "Back";

    document()->selectedModel()->updateBoundingBox();
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

void DrawArea::init(){
    /// Background color from settings file
    QString key = "DefaultBackgroundColor";
    
    /// For snapshots, default color must transparent!!
    QColor white_transp(255,255,255,0);
    settings()->setDefault( key, QVariant(white_transp) );
    setBackgroundColor( settings()->getQColor(key) );

    camera()->setUpVector(Vec(0,1,0));

    resetViewport();
}

void DrawArea::draw(){
    glEnable(GL_MULTISAMPLE); ///< Enables anti-aliasing

    /// Render each Model
    /// @todo use plugin rendering if one is specified
    glPushMatrix();
        glMultMatrixd( document()->transform.data() );
        foreach(Model* model, document()->models())
            if(model->isVisible && model->renderer()!=NULL ){ 
                qglColor(model->color);
                model->renderer()->render();
            }
    glPopMatrix();

    /// Buffers
    if(captureDepthBuffer) depth_buffer = (GLfloat*)readBuffer(GL_DEPTH_COMPONENT, GL_FLOAT);

    /// @todo Render decoration plugins
    glPushMatrix();
        glMultMatrixd( document()->transform.data() );

        /// @todo use the plugin decorators
        if(mainWindow()->hasModePlugin() && !mainWindow()->isModePluginSuspended())
            mainWindow()->getModePlugin()->decorate();            
        
        /// @todo use the standard decorators
        else{
            foreach(Model* model, document()->models()){
                if(model->isVisible) 
                    foreach(DecoratePlugin* decorator, model->decoratePlugins())
                        decorator->decorate();
            }
        }
    glPopMatrix();        

        
    /// Render mode decoration
    if( mainWindow()->hasModePlugin() )
        mainWindow()->getModePlugin()->decorate();
    
    /// Render renderable objects
    drawAllRenderObjects();
}

void DrawArea::drawWithNames(){
    if(mainWindow()->hasModePlugin())
        mainWindow()->getModePlugin()->drawWithNames();
}

std::vector< std::vector<float> > DrawArea::depthBuffer()
{
    int h = height(), w = width();
    std::vector< std::vector<float> > dbuffer(h, std::vector<float>(w,0));

    captureDepthBuffer = true;
    updateGL();

    if(!depth_buffer) return dbuffer;

    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            uint idx = ((y*w)+x);
            dbuffer[y][x] = ((GLfloat*)depth_buffer)[idx];
        }
    }

    delete depth_buffer;

    captureDepthBuffer = false;

    return dbuffer;
}

void DrawArea::endSelection(const QPoint & p)
{
    if(mainWindow()->hasModePlugin())
        mainWindow()->getModePlugin()->endSelection(p);
	else
		QGLViewer::endSelection(p);
}

void DrawArea::postSelection(const QPoint & p)
{
    if(mainWindow()->hasModePlugin())
        mainWindow()->getModePlugin()->postSelection(p);
}

void* DrawArea::readBuffer(GLenum format, GLenum type)
{
    void * data = NULL;
    int w = this->width(), h = this->height();
    switch(format)
    {
    case GL_DEPTH_COMPONENT:
        data = new GLfloat[w*h];
        break;

    case GL_RGBA:
        data = new GLubyte[w*h*4];
        break;
    }
    glReadPixels(0, 0, w, h, format, type, data);
    return data;
}

DrawArea::~DrawArea(){
    // qDebug() << "StarlabDrawArea::~StarlabDrawArea()";
    deleteAllRenderObjects();
}

void DrawArea::setRenderer(Model *model, QString name){
    // qDebug("StarlabDrawArea::setRenderer(%s,%s)",qPrintable(model->name), qPrintable(pluginName));
    document()->pushBusy();
        RenderPlugin* plugin = pluginManager()->getRenderPlugin(name);
        model->setRenderer( plugin );
    document()->popBusy();
}

void DrawArea::deleteAllRenderObjects(){
    // Render objects are not part of the document
    // document()->pushBusy();
        qDeleteAll(renderObjectList.begin(), renderObjectList.end());
        renderObjectList.clear();
    // document()->popBusy();
}

void DrawArea::drawAllRenderObjects(){
    foreach(RenderObject::Base* obj, renderObjectList)
        obj->draw();
}

void DrawArea::addRenderObject(RenderObject::Base * obj)
{
    renderObjectList.append(obj);
}


/// @todo add polygon drawing?
#if 0
for(uint i=0; i<poly.size()-2; i++){
    Point_2 p0 = poly[0]; // pivot 
    Point_2 p1 = poly[(i+1)%poly.size()];
    Point_2 p2 = poly[(i+2)%poly.size()];
    StarlabDrawArea::instance()->drawTriangle( toQt(p0), toQt(p1), toQt(p2) );
}
#endif

RenderObject::Triangle& DrawArea::drawTriangle(QVector3D p1, QVector3D p2, QVector3D p3, QColor color){
    RenderObject::Triangle* triangle = new RenderObject::Triangle(p1,p2,p3,color);
    addRenderObject(triangle);
    return *triangle;
}

RenderObject::Point& DrawArea::drawPoint(QVector3D p1, float size, QColor color){
    RenderObject::Point* point = new RenderObject::Point(p1,size,color);
    addRenderObject(point);
    return *point;
}

RenderObject::Segment& DrawArea::drawSegment(QVector3D p1, QVector3D p2, float size, QColor color){
    RenderObject::Segment* segment = new RenderObject::Segment(p1,p2,size,color);
    addRenderObject(segment);
    return *segment;
}

RenderObject::Ray& DrawArea::drawRay(QVector3D orig, QVector3D dir, float size, QColor color, float scale){
    RenderObject::Ray* ray = new RenderObject::Ray(orig,dir,size,color,scale);
    addRenderObject(ray);
    return *ray;
}

/// @internal returning true will prevent the drawArea plugin from intercepting the events!!!
bool DrawArea::eventFilter(QObject*, QEvent* event){
    /// If a mode is not open, pass *everything* to the drawArea plugin
    if(!mainWindow()->hasModePlugin()) return false;
    /// Same if the mode plugin is suspended
    if(mainWindow()->isModePluginSuspended()) return false;

    /// If it is open, pass it to the handlers
    ModePlugin* mode = mainWindow()->getModePlugin();
    switch(event->type()){
        case QEvent::MouseButtonRelease: return mode->mouseReleaseEvent((QMouseEvent*)event); break;
        case QEvent::MouseButtonPress:   return mode->mousePressEvent((QMouseEvent*)event); break;
        case QEvent::MouseMove:          return mode->mouseMoveEvent((QMouseEvent*)event); break;
        case QEvent::KeyPress:           return mode->keyPressEvent((QKeyEvent*)event); break;
        case QEvent::KeyRelease:         return mode->keyReleaseEvent((QKeyEvent*)event); break;
        case QEvent::Wheel:              return mode->wheelEvent((QWheelEvent*)event); break;
        case QEvent::MouseButtonDblClick:return mode->mouseDoubleClickEvent((QMouseEvent *)event); break;
        default: return false;
    }
}

void DrawArea::mouseDoubleClickEvent(QMouseEvent *e)
{
    bool found = false;
    qglviewer::Vec p = camera()->pointUnderPixel(e->pos(), found);

    if(found){
        camera()->setRevolveAroundPoint(p);
        camera()->lookAt(p);
        // Log the position
        mainWindow()->setStatusBarMessage(QString("Arcball centered at x = %1 | y = %2 | z = %3").arg(p.x).arg(p.y).arg(p.z));
    } 
    
#if 0 
    // Regaular behavior when clicking background
    else {
        QGLViewer::mouseDoubleClickEvent(e);
        camera()->setRevolveAroundPoint(this->sceneCenter());
        camera()->showEntireScene();        
    }
#endif
}

void DrawArea::deleteRenderObject(RenderObject* /*object*/){
    /// @todo 
    throw StarlabException("TODO: StarlabDrawArea::deleteRenderObject");
}
