#pragma once
#include "CloudPlugins.h"
class cloud_filter_example : public CloudFilterPlugin{
    Q_OBJECT
    Q_INTERFACES(FilterPlugin)

public:
    QString name() { return "cloud_filter_example (Normalize)"; }
    QString description() { return "Normalizes the point cloud to unitary bounding box"; }
    QKeySequence shortcut(){ return QKeySequence(Qt::CTRL + Qt::Key_E); }    
    void applyFilter(RichParameterSet*);
    void initParameters(RichParameterSet*);
};
