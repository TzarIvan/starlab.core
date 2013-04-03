#pragma once
#include "ModePlugin.h"

class mode_object : public ModePlugin{
    Q_OBJECT
    Q_INTERFACES(ModePlugin)
    
    /// @{ StarlabPlugin
    QString name() { return "Object Mode"; }
    QIcon icon(){ return QIcon(":/icons/mode_object.png"); }
    bool isApplicable(){  return true; }
    /// @}
    
    /// @{ ModePlugin
    void create(){}
    void destroy(){}
    bool keyReleaseEvent(QKeyEvent*);
    /// @}    

    /// @{ Local stuff
    QString command;   
    void draw();
public slots:
    void execute(QString command);
    /// @}
    
};
