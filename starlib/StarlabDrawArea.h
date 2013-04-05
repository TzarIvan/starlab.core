#pragma once
#include <QObject>
#include <QList>
#include <QVector3D>
#include <QColor>

#include "qglviewer/qglviewer.h"
#include "interfaces/RenderPlugin.h"
#include "starlib_global.h"
#include "RenderObject.h"
#include "StarlabMainWindow.h"
#include "starlib_global.h"

class QGLWidget;

namespace Starlab{

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
class STARLIB_EXPORT DrawArea : public QGLViewer{
    Q_OBJECT
    
/// @{ static instancing
private:
    static DrawArea* _staticInstance;
public:
    /// @return Returns a pointer to the application's DrawArea instance.
    ///         if no instance has been allocated, NULL is returned
    static DrawArea* instance(){ return _staticInstance; }
/// @}    
    
/// @{ core
public:
    /// Constructor
    DrawArea(MainWindow* mainWindow);
    /// Polymorphic destructor
    ~DrawArea();
private:
    MainWindow* const _mainWindow;
public:
    MainWindow* mainWindow(){ return _mainWindow; }
    Document* document(){ return _mainWindow->document(); }
    Settings* settings(){ return _mainWindow->settings(); }
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
    void init();              ///< Initializes the scene (qGLViewer)
    void draw();              ///< Draws the whole scene
    void draw_models();       ///< Draws the models in the scene; draw() subroutine
    void drawWithNames();     ///< Draws to the symbols buffer
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
public slots:
    /// Removes all render objects from the scene
    void deleteAllRenderObjects();    
public:
    void drawAllRenderObjects();
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

} // namespace

/// Concatenated Namespace
typedef Starlab::DrawArea StarlabDrawArea;
