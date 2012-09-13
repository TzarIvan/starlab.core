#include "gui_mode.h"
#include "interfaces/ModePlugin.h"
#include "StarlabMainWindow.h"
#include "StarlabDrawArea.h"

void gui_mode::load(){
    modeActionGroup = new QActionGroup(mainWindow()->modeMenu);
    modeActionGroup->setExclusive(true);
    
    /// Default mode (rendering)
    defaultModeAction = new QAction (QIcon(":/images/no_edit.png"),"Default", this);
    defaultModeAction->setShortcut(Qt::Key_Escape);
    defaultModeAction->setCheckable(true);
    defaultModeAction->setChecked(true);
    
    connect(modeActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(startMode(QAction*)));
    connect(document(),SIGNAL(selectionChanged(Model*)),this,SLOT(selectionChanged(Model*)));
}

void gui_mode::update(){
    /// Clear the menus
    mainWindow()->modeToolbar->clear();
    mainWindow()->modeMenu->clear();

    /// Add the "default" mode action (modes disactivated)
    mainWindow()->modeToolbar->addAction(defaultModeAction);
    mainWindow()->modeMenu->addAction(defaultModeAction);
    modeActionGroup->addAction(defaultModeAction);
    
    /// Re-fill the menu with plugin names and make connections
    foreach(ModePlugin* plugin, pluginManager()->editPlugins.values()){
        if(!plugin->isApplicable(document())) continue;
        
        QAction* action = plugin->action();
        action->setCheckable(true);
        modeActionGroup->addAction(action);
        mainWindow()->modeMenu->addAction(action);
        if(!action->icon().isNull())
            mainWindow()->modeToolbar->addAction(action);
    }    
}

void gui_mode::startMode(QAction* action){
    qDebug("gui_mode::startMode(%s)", qPrintable(action->text()));
    
    /// Recover the plugin that requested opening
    ModePlugin* iMode = qobject_cast<ModePlugin*>(action->parent());
    
    if( action==defaultModeAction ){
        mainWindow()->setActiveMode(NULL);
        drawArea()->setMouseBinding(Qt::LeftButton, QGLViewer::CAMERA, QGLViewer::ROTATE);
    } else {
        /// Create new plugin
        if( !mainWindow()->hasActiveMode() ){
            iMode->create();
            mainWindow()->setActiveMode(iMode);
            drawArea()->updateGL();
        }
    }
}

void gui_mode::selectionChanged(Model* model){
    qDebug("gui_mode::selectionChanged(%s)", qPrintable(model->name));
    ModePlugin* iMode = mainWindow()->activeMode();
    
    if( !mainWindow()->hasActiveMode() ) return;
    
    /// Give the plugin a chance to react to the selection change
    bool updatePerformed = iMode->selectionChanged(model);
    
    /// If plugin wasn't able to perform the update, simply destroy
    /// the plugina and re-create it from scratch!
    if(!updatePerformed){
        iMode->destroy();
        iMode->create();
    }
}

Q_EXPORT_PLUGIN(gui_mode)
