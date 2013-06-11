#pragma once
#include "ModePlugin.h"
#include "StarlabController.h"

/// A specialized version of a mode plugin that uses "GUI Controllers".
/// These are editable GUI elements with whom the user can interact.
class ControllerModePlugin : public ModePlugin{
protected:
    Starlab::ControllerManager manager;
    
/// @{ 
    ControllerModePlugin() : manager(this){}
    bool mousePressEvent(QMouseEvent* event){ return manager.mousePressEvent(event); }
    bool mouseReleaseEvent(QMouseEvent* event){ return manager.mousePressEvent(event); }
    bool mouseMoveEvent(QMouseEvent* event){ return manager.mouseMoveEvent(event); }
    void decorate(){ manager.draw(); }    
    void drawWithNames(){ manager.drawWithNames(); }
/// @}
    
/// @{ internal
    /// Example, in create(): "controllers() << new FrameController( Vector3(0,0,0) );"
    Starlab::ControllerManager& operator<<(StarlabController* controller);
    /// Return a reference to allow for the syntax: "controllers() << new Controller(...)"
    Starlab::ControllerManager& controllers(){ return manager; }
/// @}
    
};