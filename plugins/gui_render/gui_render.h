#pragma once
#include <QToolBar>

#include "StarlabMainWindow.h"
#include "interfaces/GuiPlugin.h"
#include "interfaces/RenderPlugin.h"
#include "ParametersFrame.h"

class gui_render : public GuiPlugin{
    Q_OBJECT
    Q_INTERFACES(GuiPlugin)
    
/// @{ updates GUI according to selection
public slots:
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
    void trigger_editSettings();
private:
    QActionGroup* renderModeGroup;
    QAction* currentAsDefault;
    QAction* editRenderSettings;
/// @}
};

/// @internal Q_OBJECT classes must be declared in an .h file for MOC'ing
class ParametersWidget: public QWidget {
    Q_OBJECT
public:
    ParametersWidget(RichParameterSet* pars, QWidget* parent=NULL, Qt::WindowFlags flags = 0) 
        : QWidget(parent, flags), _frame(this, flags)
    {
        /// Load parameters
        _frame.load(pars);

        /// Add to widget
        setLayout( new QVBoxLayout );
        layout()->addWidget(&_frame);
        
        /// Auto-delete me on close
        this->setAttribute(Qt::WA_DeleteOnClose, true);
        
        /// Forward signal
        connect(&_frame, SIGNAL(parameterChanged()), this, SIGNAL(parameterChanged()));
    }
private:
    ParametersFrame _frame;
signals:
    void parameterChanged();
};
