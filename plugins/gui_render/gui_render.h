#pragma once
#include <QToolBar>

#include "StarlabMainWindow.h"
#include "interfaces/GuiPlugin.h"
#include "interfaces/RenderPlugin.h"

class gui_render : public GuiPlugin{
    Q_OBJECT
    Q_INTERFACES(GuiPlugin)
    
    /// @{ updates GUI according to selection
    public:    
        void load();    
        void update();
    /// @}
    
    /// @{ access specific resources
        QToolBar* toolbar(){ return mainWindow()->renderToolbar; }
        QMenu* menu(){ return mainWindow()->renderMenu; }
    /// @}
        
    /// @{ renderer module
    public slots:
        void triggerRenderModeAction(QAction* );
        void triggerSetDefaultRenderer();
    private:
        QActionGroup* renderModeGroup;
        QAction* currentAsDefault;
    /// @}
};
