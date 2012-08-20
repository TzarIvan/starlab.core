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
        void resetViewport(); /// Restores the default view camera
    private:
        void init(); 
        void draw(); ///< Draws the whole scene
        void drawWithNames();
    /// @}
        
    /// @{ ModePlugin management
    private:
        bool eventFilter(QObject *object, QEvent *event);
    /// @}
        
    /// @{ Model rendering system
    private:
        /// Stores the association model/renderer
        QMap<Model*, RenderPlugin*> _renderers;
    public:
        /// Retrieves the list of renderers and initialize if necessary
        QList<RenderPlugin*> renderers();
        /// Set the renderer for the model by name
        RenderPlugin* activeRenderer(Model* model);
        void setRenderer(Model *model, QString pluginName);
    /// @}
    
    /// @{ Scene-wide rendering elements (i.e. used for debug)
    public:
        void drawAllRenderObjects();
        void deleteAllRenderObjects();
        void deleteRenderObject(RenderObject* );
        RenderObject::Triangle& drawTriangle(QVector3D p1, QVector3D p2, QVector3D p3, QColor color=Qt::red);
        RenderObject::Point&    drawPoint(QVector3D p1, float size=1, QColor color=Qt::red);
        RenderObject::Segment&  drawSegment(QVector3D p1, QVector3D p2, float size=1, QColor color=Qt::red);
        RenderObject::Ray&      drawRay(QVector3D orig, QVector3D dir, float size=1, QColor color=Qt::red, float scale=1);
    
    protected:        
        /// Handles to additional render objects, created by draw??() methods, deleted by ~StarlabDrawArea()
        QList<RenderObject::Base*> renderObjectList;
    /// @}
              
public slots:
            
    /// Updates the draw area
    void update();
};

/// Old implementation
//class StarlabDrawArea{   
//public slots:
//    /// Copies the view matrix to the system clipboard
//    virtual void viewToClipboard(){}
//    /// Pastes the view matrix to the system clipboard
//    virtual void viewFromClipboard(){}
//    /// Simple viewpoint selection
//    virtual void viewFrom(QAction* /*from*/=NULL){}
//    /// Selects a simple view setting (MESHLAB: createOrthoView)
//    virtual void selectSimpleView(QString /*viewname*/){}
//};
