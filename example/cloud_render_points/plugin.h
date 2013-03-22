#pragma once
#include "CloudPlugins.h"
#include "PluginManager.h"

class plugin : public CloudRenderPlugin{
    Q_OBJECT
    Q_INTERFACES(RenderPlugin)
public: 
    QString name() { return "Point Cloud"; }
    QIcon icon(){ return QIcon(":/points.png"); }
    Renderer* instance();
protected:
    virtual bool isDefault() { return true; }
};
