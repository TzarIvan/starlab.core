#pragma once
#include <QObject>
#include <QList>
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

/// @ingroup starcore
/// @brief The QGLViewer based visualization window
class STARLIB_EXPORT DrawArea : public QGLViewer{
    Q_OBJECT
    
    typedef Eigen::Vector3d Vector3;

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
    /// @obsolete see clear()
    void deleteAllRenderObjects(){ qWarning("deleteAllRenderObject() obsolete!"); clear(); }
    /// @brief Removes all debug render objects from the scene
    void clear();
public:
    void drawAllRenderObjects();
    void deleteRenderObject(RenderObject* );
    void addRenderObject(RenderObject::Base*);
    RenderObject::Triangle& drawTriangle(Vector3 p1, Vector3 p2, Vector3 p3, QColor color=Qt::red);
    RenderObject::Point&    drawPoint(Vector3 p1, float size=1, QColor color=Qt::red);
    RenderObject::Segment&  drawSegment(Vector3 p1, Vector3 p2, float size=1, QColor color=Qt::red);
    RenderObject::Ray&      drawRay(Vector3 orig, Vector3 dir, float size=1, QColor color=Qt::red, float scale=1);
    RenderObject::Text&     drawText(int x, int y, const QString& text, float size=12, QColor color=Qt::red);
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
