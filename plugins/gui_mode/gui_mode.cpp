#include <QKeyEvent>
#include <QApplication>
#include "gui_mode.h"

#include "interfaces/ModePlugin.h"
#include "StarlabMainWindow.h"
#include "StarlabDrawArea.h"

void gui_mode::load(){
    lastActiveModeAction = NULL;
    
    modeActionGroup = new QActionGroup(mainWindow()->modeMenu);
    /// Managed manually
    modeActionGroup->setExclusive(false);
    
    /// Default mode (Trackball)
    defaultModeAction = new QAction (QIcon(":/images/no_edit.png"),"Default", this);
    defaultModeAction->setCheckable(true);
    defaultModeAction->setShortcut(Qt::Key_Escape);
    
    /// Reacts to changes in mode
    connect(modeActionGroup,SIGNAL(triggered(QAction*)),this,SLOT(actionClicked(QAction*)));
    
    /// Reacts to changes made on the selection
    connect(document(),SIGNAL(hasChanged()),this,SLOT(documentChanged()));
    /// When document changes, we make sure render menu/toolbars are up to date    
    connect(document(), SIGNAL(hasChanged()), this, SLOT(update()));
    
    /// Start state machine in default mode
    enterState(DEFAULT,defaultModeAction);
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
    foreach(ModePlugin* plugin, pluginManager()->modePlugins()){
        if(!plugin->isApplicable()) continue;
        QAction* action = plugin->action();
        action->setCheckable(true);
        /// Make GUI elements exclusive
        modeActionGroup->addAction(action);
        /// Add to menus and toolbars
        mainWindow()->modeMenu->addAction(action);
        if(!action->icon().isNull())
            mainWindow()->modeToolbar->addAction(action);
    }
    
    /// Remember trackball is always there, thus>1
    bool showtoolbar = (mainWindow()->modeToolbar->children().size() > 1);
    mainWindow()->modeToolbar->setVisible(showtoolbar);
}

void gui_mode::enterState(STATE state, QAction* action){
    switch(state){
    case DEFAULT: 
        // qDebug() << "[DEFAULT]";
        Q_ASSERT(lastActiveModeAction==NULL);
        Q_ASSERT(!mainWindow()->hasModePlugin());
        foreach(QAction* action, modeActionGroup->actions())
            action->setEnabled(true);
        defaultModeAction->setChecked(true);
        defaultModeAction->setEnabled(false);
        break;
    case MODE: 
        // qDebug() << "[MODE]";
        Q_ASSERT(mainWindow()->hasModePlugin());
        foreach(QAction* action, modeActionGroup->actions())
            action->setEnabled(false);
        defaultModeAction->setEnabled(true);
        defaultModeAction->setChecked(false);
        action->setEnabled(true);
        action->setChecked(true);
        break;
    case SUSPENDED: 
        // qDebug() << "[SUSPENDED]";
        Q_ASSERT(mainWindow()->hasModePlugin());
        Q_ASSERT(lastActiveModeAction!=NULL);
        foreach(QAction* action, modeActionGroup->actions())
            action->setEnabled(false);        
        defaultModeAction->setChecked(true);
        defaultModeAction->setEnabled(true);
        lastActiveModeAction->setChecked(true);
        lastActiveModeAction->setEnabled(false);
        break;
    }    
    
    /// Finally update state
    this->state = state;
}

void gui_mode::actionClicked(QAction *action){
    // qDebug() << QString("gui_mode::actionClicked(%1)").arg(action->text());
    
    switch(state){
    case DEFAULT:
        /// ---------------- IGNORING --------------------
        if(action==defaultModeAction)
            return;
        /// ---------------- CREATING --------------------
        if(action!=defaultModeAction){
            mainWindow()->setModePlugin( (ModePlugin*) action->parent() );
            mainWindow()->getModePlugin()->create();
            mainWindow()->resumeModePlugin();
            drawArea()->updateGL();
            lastActiveModeAction = action;
            enterState(MODE,action);
            showMessage("Creating plugin: '%s'",qPrintable(action->text()));
            return;              
        }
        break;
    case MODE: 
        /// ---------------- TERMINATION --------------------
        if(action==lastActiveModeAction){
            Q_ASSERT(mainWindow()->getModePlugin()!=NULL);
            mainWindow()->getModePlugin()->destroy();
            mainWindow()->removeModePlugin();
            lastActiveModeAction = NULL;
            enterState(DEFAULT);
            showMessage("Terminated plugin: '%s'",qPrintable(action->text()));
            return;            
        }
        /// ---------------- SUSPENSION --------------------
        if(action==defaultModeAction){
            QAction* actionToSuspend = lastActiveModeAction;
            ModePlugin* pluginToSuspend = (ModePlugin*) lastActiveModeAction->parent();
            mainWindow()->suspendModePlugin();
            pluginToSuspend->suspend();
            enterState(SUSPENDED,actionToSuspend);
            showMessage("Suspended plugin: '%s'",qPrintable(actionToSuspend->text()));
            return;
        }
        break;
    case SUSPENDED: 
        /// ---------------- RESUMING --------------------
        Q_ASSERT(action==defaultModeAction);
        mainWindow()->resumeModePlugin();
        ((ModePlugin*) lastActiveModeAction->parent())->resume();
        enterState(MODE,lastActiveModeAction);
        showMessage("Resumed plugin: '%s'",qPrintable(lastActiveModeAction->text()));
        break;
    }
}

void gui_mode::documentChanged(){  
    // qDebug("gui_mode::documentChanged()");
    if(!mainWindow()->hasModePlugin()) return;
    ModePlugin* iMode = mainWindow()->getModePlugin();

    switch(state){
    /// But there was no active plugin
    case DEFAULT:
        return;
    /// And there was an active plugin
    case MODE:
        /// Give the plugin a chance to react to the selection change
        /// If plugin didn't specify how to perform the update, simply 
        /// destroy it and re-create it from scratch.
        if(!iMode->documentChanged()){
            iMode->destroy();
            iMode->create();
        }
        return;
    /// There was a suspended plugin
    case SUSPENDED:
        /// On the other hand, when plugin is suspended, change in document just 
        /// results in the plugin termination
        if(!iMode->documentChanged())
            iMode->destroy();
        enterState(DEFAULT);
        return;    
    }
}


Q_EXPORT_PLUGIN(gui_mode)
