#pragma once
#include "CloudPlugins.h"
#include "PluginManager.h"

class cloud_render_points : public CloudRenderPlugin{
    Q_OBJECT
    Q_INTERFACES(RenderPlugin)
public: 
    RenderPlugin* factory(){ return new cloud_render_points(); }
    QString name() { return "Point Cloud"; }
    QIcon icon(){ return QIcon(":/points.png"); }
    void render();
protected:
    virtual bool isDefault() { return true; }
};
