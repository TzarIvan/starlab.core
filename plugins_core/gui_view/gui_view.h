#pragma once
#include <QMenu>
#include "interfaces/GuiPlugin.h"
#include "interfaces/DrawAreaPlugin.h"
#include "StarlabMainWindow.h"
#include "StarlabDrawArea.h"

class gui_view : public GuiPlugin{
    Q_OBJECT
    Q_INTERFACES(GuiPlugin)

    /// @{ split load in pieces
    public:
        void load();
    private:
        void load_selectdrawarea();
        void load_viewfrom();
        void load_fullscreen();
        void load_copypasteviewmatrix();
    /// @}    
    
private slots:   
    /// Loads the drawArea associated with the action
    void selectDrawArea(QAction* action);
};
