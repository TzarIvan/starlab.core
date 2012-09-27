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
        qDebug() << "example_mode_withwidget::::destroyed()";
        dockwidget->deleteLater(); 
    }
    void suspend(){ 
        qDebug() << "example_mode_withwidget::suspended()";    
        dockwidget->setEnabled(false); 
    }
    void resume(){ 
        qDebug() << "example_mode_withwidget::resumed()";    
        dockwidget->setEnabled(true); 
    }
    bool documentChanged(){
        qDebug() << "example_mode_withwidget::documentChanged()";
        return true;        
    }
/// @} 

/// @{ Local data    
    ModePluginDockWidget* dockwidget;
/// @} 
};