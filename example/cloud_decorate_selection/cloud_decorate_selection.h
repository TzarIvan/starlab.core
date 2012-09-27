#pragma once
#include "CloudPlugins.h"
class cloud_decorate_selection : public CloudDecoratePlugin{
    Q_OBJECT
    Q_INTERFACES(DecoratePlugin)
    
    QIcon icon(){ return QIcon(":/cloud_decorate_selection.png"); }
    QString name() { return "Highlight selection"; }
    DecoratePlugin* factory(){ return new cloud_decorate_selection(); }  
    void decorate();
};
