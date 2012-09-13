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
    QActionGroup* modeActionGroup;
    
    /// plugin constructor
    void load();
    void update();
    
public slots:
    /// Starts a mode plugin (also manages the suspend)
    void startMode(QAction *);

    /// Responds to a change in selection. If the plugin specifies its own way 
    /// to respond to the event, this is used. This can be done by overloading 
    /// ModePlugin::selectionChanged(Model* model). If no custom behavior is 
    /// provided, we simply call ModePlugin::destroy(), ModePlugin::create() 
    /// in succession
    void selectionChanged(Model* model);
};
