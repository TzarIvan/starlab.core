#pragma once
#include "interfaces/GuiPlugin.h"
#include "interfaces/ModePlugin.h"

/// At any point there can be a single editing plugin active.
/// When a plugin is active (i.e. not suspended) it intercept 
/// all the input (i.e. mouse/keyboard) actions.
class gui_mode : public GuiPlugin{
    Q_OBJECT
    Q_INTERFACES(GuiPlugin)
    
public:    

    QAction* defaultModeAction; 
    gui_mode(){ 
        defaultModeAction = new QAction (QIcon(":/images/no_edit.png"),"Default", this);
    }

    /// plugin constructor
    virtual void load();
    
public slots:
    /// Starts an edit plugin, popping up its default GUI (also manages the suspend)
    void startMode(QAction *);
};
