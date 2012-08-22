#include "gui_mode.h"
#include "interfaces/ModePlugin.h"
#include "StarlabMainWindow.h"

void gui_mode::load(){
    QActionGroup* modeActionGroup = new QActionGroup(mainWindow()->modeMenu);
    modeActionGroup->setExclusive(true);
    
    /// Default mode (rendering)
    defaultModeAction = new QAction (QIcon(":/images/no_edit.png"),"Default", this);
    defaultModeAction->setShortcut(Qt::Key_Escape);
    defaultModeAction->setCheckable(true);
    defaultModeAction->setChecked(true);
    mainWindow()->modeToolbar->addAction(defaultModeAction);
    mainWindow()->modeMenu->addAction(defaultModeAction);
    modeActionGroup->addAction(defaultModeAction);
    
    /// Fill the menu with plugin names and make connections
    foreach(ModePlugin* plugin, pluginManager()->editPlugins.values()){
        QAction* action = plugin->action();
        action->setCheckable(true);
        modeActionGroup->addAction(action);
        mainWindow()->modeMenu->addAction(action);
        if(!action->icon().isNull())
            mainWindow()->modeToolbar->addAction(action);
    }

    connect(modeActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(startMode(QAction*)));
}

void gui_mode::startMode(QAction* action){
    qDebug("gui_mode::startMode(%s)", qPrintable(action->text()));
    
    /// Recover the plugin that requested opening
    ModePlugin* iMode = qobject_cast<ModePlugin*>(action->parent());
    
    if( action==defaultModeAction ){
        mainWindow()->setActiveMode(NULL);
    } else {
        /// Create new plugin
        if( !mainWindow()->hasActiveMode() ){
            iMode->createEdit();
            mainWindow()->setActiveMode(iMode);
        }
    }
}

Q_EXPORT_PLUGIN(gui_mode)
