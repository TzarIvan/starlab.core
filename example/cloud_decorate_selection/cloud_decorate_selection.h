#pragma once
#include "CloudPlugins.h"
class cloud_decorate_selection : public CloudDecoratePlugin{
    Q_OBJECT
    Q_INTERFACES(DecoratePlugin)
    
    QString name() { return "Decorate Selection"; }
    DecoratePlugin* factory(){ return new cloud_decorate_selection(); }  
    void decorate();

};
