#pragma once
#include <QDebug>
#include <QString>
#include <QObject>
#include <QColor>
#include "qt3d/math3d/qbox3d.h"
#include "starlib_global.h"

class DecoratePlugin;
class RenderPlugin;
class QTreeWidgetItem;
class StarlabDrawArea;

/// Interface of the basic characteristics that every model needs to have
class STARLIB_EXPORT Model : public QObject{
    Q_OBJECT

/// @{ Overload these to obtain a BASIC new datatype
public:
    /// Basic constructor
    Model(QString path, QString name=QString());
    
    /// Compute the bounding box for the model
    /// @internal necessary because we have to compute the BBOX of the scene
    virtual void updateBoundingBox() = 0;
/// @}
            
/// @{ Overload these to customize appearance in the Layers dialog
public:
    /// A class that describes this class of models in the layer editor. The 
    /// default implementation doesn't specify anything, meaning the layerDialog
    /// will create something very basic. Rather than overriding this method you 
    /// should consider using its default implementation and override "decorateLayersWidgedItem"
    virtual QTreeWidgetItem* getLayersWidgetItem(){ return NULL; }
    
    /// This takes the layer item of getLayersWidgetItem() and adds a bunch 
    /// of decorations on top of it. It is easier to customize this entry
    /// in your custom made model
    /// The default implementation just shows the full path as first entry
    virtual void decorateLayersWidgedItem(QTreeWidgetItem* parent);
/// @}
    
public:
    bool isVisible;       /// is the current model visible?
    bool isModified;      /// has the current model been modified (and not saved)? @todo make private and setter/getter to signal
    QString name;         /// name of the model (to be used in layer pane)
    QString path;         /// full path of the file associated to the model 
    QColor color;         /// Basic color (handy to see bboxes for example)
    /// @todo use the QGLViewer transformation type here
    // QMatrix4x4 transform; /// Per-model transformation (i.e. alignment matrix in a scan)

/// @{ Rendering 
public: 
    RenderPlugin* renderer();
    void setRenderer(RenderPlugin *plugin);
private:
    RenderPlugin* _renderer;
/// @}

/// @{ Decorators
public:
    typedef QList<DecoratePlugin*> DecoratePlugins;
    const DecoratePlugins& decoratePlugins(){ return _decoratePlugins; }
    void removeDecoratePlugin(DecoratePlugin *plugin);
    void addDecoratePlugin(DecoratePlugin *plugin);
    bool hasDecoratePlugin(DecoratePlugin *plugin);
private:
    DecoratePlugins _decoratePlugins;
/// @}
    
/// @{ Basic geometry, needed for rendering
public:
    const QBox3D& bbox(){ return _bbox; }
protected: 
    /// Per-model bounding box (before transform is applied)
    QBox3D _bbox;
/// @}        
};

Q_DECLARE_INTERFACE(Model, "starlab.interface.Model/1.0")