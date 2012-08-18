#pragma once
#include "CloudPlugins.h"

class cloud_render_points : public RenderPlugin{
    Q_OBJECT
    Q_INTERFACES(RenderPlugin)
public: 
    RenderPlugin* factory(){ return new cloud_render_points(); }
    QString name() { return "Point Cloud"; }
    bool isApplicable(Model* model);
    QIcon icon(){ return QIcon(":/points.png"); }
    void render();
};
