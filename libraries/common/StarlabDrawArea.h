#pragma once
#include <QObject>
#include <QList>
#include <QVector3D>
#include <QColor>

#include <qglviewer.h>
#include "interfaces/RenderPlugin.h"
#include "dynamic_common_global.h"
#include "RenderObject.h"
#include "StarlabMainWindow.h"

class Document;
class StarlabSettings;
class QGLWidget;
class Model;

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

/// @brief The default behavior that ANY DrawArea plugin should expose to the system
///
/// @internal The problem needs to be solved with encapsulation. Unfortunately
/// we cannot have StarlabDrawArea inherit from QGLWidget as it would cause
/// multiple inheritance issues to appear with QGLViewer or GLArea. 
/// On aftertought this makes sense, as for example we are not allowed to modify 
/// GLArea from meshlab, but nothing stops us from having a wrapper around it!!!
/// Doing a wrapper also gives us the chance of using it as a Q_OBJECT
class DYNAMIC_COMMON_EXPORT StarlabDrawArea : public QGLViewer{
    Q_OBJECT
    
/// @{ core
public:
    /// Constructor
    StarlabDrawArea(StarlabMainWindow* mainWindow);
    /// Polymorphic destructor
    ~StarlabDrawArea();
private:
    StarlabMainWindow* const _mainWindow;
public:
    StarlabMainWindow* mainWindow(){ return _mainWindow; }
    Document* document(){ return _mainWindow->document(); }
    StarlabSettings* settings(){ return _mainWindow->settings(); }
    PluginManager* pluginManager(){ return _mainWindow->pluginManager(); }
/// @}
 
/// @{ Rendering specification
public:
    void setRenderer(Model* model, QString pluginName);
    
public slots:
    void resetViewport();     ///< Restores the default view camera
    void viewFrom(QAction *); ///< Preset camera viewpoints
    void setPerspectiveProjection();
    void setOrthoProjection();
    void setIsoProjection();
private:
    void init();              ///< Initializes the scene
    void draw();              ///< Draws the whole scene
    void drawWithNames();     ///< Draws to the symbols buffer
/// @}

/// @{ OpenGL buffers
public slots:
    std::vector< std::vector<float> > depthBuffer();
private:
    void *readBuffer(GLenum format, GLenum type);
    void *depth_buffer;
    bool captureDepthBuffer;
/// @}

/// @{ Selection functions
private:
    void endSelection(const QPoint&);
    void postSelection(const QPoint&);
public:
	void defaultEndSelection(const QPoint&p){ QGLViewer::endSelection(p); } ///< Needed in Mode plugins
/// @}
    
/// @{ ModePlugin management
private:
    bool eventFilter(QObject *object, QEvent *event);
/// @}

/// @{ Default mouse & keyboard management
    void mouseDoubleClickEvent( QMouseEvent * e );
/// @}
        
/// @{ Scene-wide rendering elements (i.e. used for debug)
public:
    void drawAllRenderObjects();
    void deleteAllRenderObjects();
    void deleteRenderObject(RenderObject* );
    void addRenderObject(RenderObject::Base*);
    RenderObject::Triangle& drawTriangle(QVector3D p1, QVector3D p2, QVector3D p3, QColor color=Qt::red);
    RenderObject::Point&    drawPoint(QVector3D p1, float size=1, QColor color=Qt::red);
    RenderObject::Segment&  drawSegment(QVector3D p1, QVector3D p2, float size=1, QColor color=Qt::red);
    RenderObject::Ray&      drawRay(QVector3D orig, QVector3D dir, float size=1, QColor color=Qt::red, float scale=1);
/// @}
    
public slots:
    /// Updates the draw area, this is not the OpenGL update, this updates all
    /// the metadata needed by all models for correct rendering!! 
    void update();

protected:
    /// Handles to additional render objects, created by draw??() methods, deleted by ~StarlabDrawArea()
    QList<RenderObject::Base*> renderObjectList;
};
