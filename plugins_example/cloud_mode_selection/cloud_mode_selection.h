#pragma once
#include "interfaces/ModePlugin.h"

class cloud_mode_selection : public ModePlugin{
    Q_OBJECT
    Q_INTERFACES(ModePlugin)

    /// Functions part of the EditPlugin system
    virtual void createEdit(){}
    virtual void destroyEdit(){}    

    void DrawXORRect();
};
