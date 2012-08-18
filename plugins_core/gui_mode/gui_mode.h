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
    /// plugin constructor
    virtual void load();
    
private:   
    /// the action for the suspend mode (::load)
    // QAction* suspendEditModeAct;
    /// Stores the actions (menu entries and buttons) associated with a plugin
    QMap<StarlabPlugin*, QAction*> pluginToActionMap;       
    /// Keeps track of the active plugin to avoid duplicated GUI
    /// This field is NULL unless a suspended Edit plugin exists
    ModePlugin* activeEditPlugin;
    
public slots:
    /// Starts an edit plugin, popping up its default GUI (also manages the suspend)
    void startEdit();
    void endEdit();
    void suspendEdit();
};
