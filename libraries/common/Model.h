#pragma once
#include <QDebug>
#include <QString>
#include <QObject>
#include <QColor>
#include "qt3d/math3d/qbox3d.h"
#include "dynamic_common_global.h"

class DecoratePlugin;
class RenderPlugin;
class QTreeWidgetItem;

/// Interface of the basic characteristics that every model needs to have
class DYNAMIC_COMMON_EXPORT Model : public QObject{
    Q_OBJECT

    /// @{ Overload these for plugin
    public:
        /// Basic constructor
        Model(QString path, QString name=QString());
        virtual void updateBoundingBox() = 0;
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
    QMatrix4x4 transform; /// Per-model transformation (i.e. alignment matrix in a scan)
        
    /// @{ Basic geometry, needed for rendering
    public:
        const QBox3D& bbox(){ return _bbox; }
    protected: 
        QBox3D _bbox;          /// Per-model bounding box (before transform is applied)
    /// @}        
};

Q_DECLARE_INTERFACE(Model, "starlab.interface.Model/1.0")
