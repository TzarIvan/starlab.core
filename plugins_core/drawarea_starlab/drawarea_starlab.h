#pragma once
#include "interfaces/DrawAreaPlugin.h"

class drawarea_starlab : public DrawAreaPlugin{
    Q_OBJECT
    Q_INTERFACES(DrawAreaPlugin)

public:
    StarlabDrawArea* load();
};
