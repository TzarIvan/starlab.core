#include "gui_mode.h"
#include "interfaces/ModePlugin.h"
#include "StarlabMainWindow.h"

void gui_mode::load(){
    this->activeEditPlugin = NULL;
    
    /// Fill the "edit" menu/toolbar
    {
#ifdef OLD_MESHLAB
        suspendEditModeAct = new QAction (QIcon(":/images/no_edit.png"),tr("Not editing"), this);
        suspendEditModeAct->setShortcut(Qt::Key_Escape);
        suspendEditModeAct->setCheckable(true);
        suspendEditModeAct->setChecked(true);
        connect(suspendEditModeAct, SIGNAL(triggered()), this, SLOT(suspendEdit()));
        window->modeToolbar->addAction(suspendEditModeAct);
        window->modeToolbar->addSeparator();
#endif
    }
    
    /// Fill the menu with plugin names and make connections
    foreach(ModePlugin* plugin, pluginManager()->editPlugins.values()){
        // Action refers to this filter, so we can retrieve it later
        QAction* action = new QAction(plugin->name(),plugin);
        action->setToolTip(plugin->description());
        connect(action,SIGNAL(triggered()),this,SLOT(startEdit()));
        mainWindow()->modeMenu->addAction(action);      
        
        #if 0
            filterMenu->addAction(action);
            /// Does the filter have an icon? Add it to the toolbar
            /// @todo add functionality to ModelFilter
            if(!action->icon().isNull())
                filterToolBar->addAction(action);
        #endif
        
        /// @brief add edit icons to the toolbar
        #ifdef OLDMESHLAB
            foreach(MeshEditInterface* iMode, PM.meshEditPlugins()) {
                QAction* editAction = iMode->action;
                if(editAction && !editAction->icon().isNull())
                    editToolBar->addAction(editAction);
                else 
                    qDebug() << "icon of the Edit was null!";
            }
        #endif
        
        /// Store the actions associated with the filter
        pluginToActionMap.insert(plugin, action);
    }
}

void gui_mode::startEdit(){
    qDebug() << "gui_mode::startEdit()";
    
    /// Recover the plugin that requested opening
    QAction *action = qobject_cast<QAction*>(sender());
    ModePlugin* iMode = qobject_cast<ModePlugin*>(action->parent());
    
    /// Create new plugin
    if( activeEditPlugin==NULL ){
        /// Instantiate the edit
        iMode->createEdit();
        activeEditPlugin = iMode;
        
#if 0
        /// And if in the startup process the plugin created a widget, 
        /// open and set it up
        if( iMode->hasWidget() ){
            mainWindow()->addDockWidget(Qt::RightDockWidgetArea, iMode->getWidget());
            iMode->getWidget()->setParent(&mainWindow());
            iMode->getWidget()->show();
            iMode->getWidget()->setAttribute(Qt::WA_DeleteOnClose);
            
            /// @todo if uses closes widget, invoke endEdit()
            // iMode->getWidget()->closeEvent();
        }        
#endif
    }
    
    /// Un-suspend a plugin
    if( activeEditPlugin!=NULL && activeEditPlugin->isSuspended ){
        // suspendEditModeAct->setChecked(false);

#if 0        
        /// re-activate the widget, if it has any
        if( iMode->hasWidget() ){
            iMode->getWidget()->setEnabled(true);
        }
#endif
    }
    
    /// updateMenus();
    /// updateToolbars();
}

void gui_mode::suspendEdit(){
    qDebug() << "gui_editplugins::suspendEdit()";
    if(activeEditPlugin==NULL) return;
    
    activeEditPlugin->isSuspended = true;
    
#if 0
    if(activeEditPlugin->hasWidget())
        activeEditPlugin->getWidget()->setDisabled(true);
#endif  
    
    QAction* action = pluginToActionMap[activeEditPlugin];
    if(action!=NULL)
        action->setDisabled(true);
}

void gui_mode::endEdit(){
}

Q_EXPORT_PLUGIN(gui_mode)
