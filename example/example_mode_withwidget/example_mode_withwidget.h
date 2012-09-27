#pragma once
#include "interfaces/ModePlugin.h"
#include "StarlabMainWindow.h"

class example_mode_withwidget : public ModePlugin{
    Q_OBJECT
    Q_INTERFACES(ModePlugin)

/// @{ StarlabPlugin
    QIcon icon(){ return QIcon(":/example_mode_withwidget.png"); }
    bool isApplicable(){  return true; }
/// @}

/// @{ ModePlugin
    void create();
    void destroy(){ 
        qDebug() << "destroyed";
        dockwidget->deleteLater(); 
    }
    void suspend(){ 
        qDebug() << "suspended";    
        dockwidget->setEnabled(false); 
    }
    void resume(){ 
        qDebug() << "resumed";    
        dockwidget->setEnabled(true); 
    }
    bool documentChanged(){
        qDebug() << "documentChanged()";
        return true;        
    }
/// @} 

/// @{ Local data    
    ModePluginDockWidget* dockwidget;
/// @} 
};
