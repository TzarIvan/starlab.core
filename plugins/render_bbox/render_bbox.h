#pragma once
#include "RenderPlugin.h"

class render_bbox : public RenderPlugin{
    Q_OBJECT
    Q_INTERFACES(RenderPlugin)
public: 
    RenderPlugin* factory(){return new render_bbox(); }
    QString name() { return "Bounding Box"; }
    QIcon icon(){ return QIcon(":/icons/bbox.png"); }
    bool isApplicable(Model* model);
    void render();
};
