#pragma once
#include "interfaces/DrawAreaPlugin.h"
class StarlabMainWindow;

/// There is only one instance of this object, so it's ok to have a state
class drawarea_meshlab : public DrawAreaPlugin{
    Q_OBJECT
    Q_INTERFACES(DrawAreaPlugin)
public:
    StarlabDrawArea* load();
    
public slots:
    void toggleTwoSidedLighting(bool);
    void toggleBackground(bool);
};
