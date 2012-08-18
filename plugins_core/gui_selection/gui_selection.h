#pragma once
#include <QToolBar>

#include "StarlabMainWindow.h"
#include "StarlabDrawArea.h"
#include "interfaces/GuiPlugin.h"
#include "interfaces/RenderPlugin.h"
#include "interfaces/DecoratePlugin.h"

class gui_selection : public GuiPlugin{
    Q_OBJECT
    Q_INTERFACES(GuiPlugin)
    
    /// @{ updates GUI according to selection
    public:    
        void load();    
        void update();
    private:
        void update_renderModes();
        void update_decorators();
    /// @}
    
    /// @{ access specific resources
    private:    
        QToolBar* toolbar(){ return mainWindow()->selectionToolbar; }
        QMenu* menu(){ return mainWindow()->selectionMenu; }
    /// @}
        
    /// @{ decorator module
    public slots:
        void toggleDecorator(QAction* );
    private:
        QActionGroup* decoratorGroup;
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
