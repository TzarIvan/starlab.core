#include "gui_selection.h"
#include "StarlabDrawArea.h"

/// Since we depend on the selected model, the load is minimal
void gui_selection::load(){
    // qDebug() << "gui_selection::load()";
    this->decoratorGroup = new QActionGroup(this);
    this->decoratorGroup->setExclusive(false);
    this->renderModeGroup = new QActionGroup(this);
    this->renderModeGroup->setExclusive(true);
    this->currentAsDefault = new QAction("Set current as default...",NULL);
}

void gui_selection::update(){
    toolbar()->clear();
    menu()->clear();
    update_renderModes();
    update_decorators();
}

void gui_selection::update_renderModes(){
    QMenu* renderModeMenu = menu()->addMenu("Render Mode");
    
    /// Fetch current renderer
    Model* selectedModel = document()->selectedModel();
    RenderPlugin* currentRenderer = drawArea()->activeRenderer(selectedModel);
    
    /// Add render modes menu/toolbar entries
    foreach(QAction* action, pluginManager()->getRenderPluginsActions(selectedModel)){
        action->setCheckable(true);
        
        /// "Check" an icon
        if(currentRenderer!=NULL)
            if(currentRenderer->action() == action)
                action->setChecked("true");
        
        renderModeGroup->addAction(action);
        /// If it has icon.. add it to toolbar
        if(!action->icon().isNull())
            toolbar()->addAction(action);
    }
        
    /// @internal menu can be added only after it has been filled :(
    renderModeMenu->addAction(currentAsDefault);
    renderModeMenu->addSeparator();
    renderModeMenu->addActions(renderModeGroup->actions());
    renderModeMenu->addSeparator();
    
    /// Connect click events to change in renderer system
    /// @todo why is it triggering 3x events every time? 
    connect(renderModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(triggerRenderModeAction(QAction*)), Qt::UniqueConnection);
    connect(currentAsDefault, SIGNAL(triggered()), this, SLOT(triggerSetDefaultRenderer()));
}

void gui_selection::update_decorators(){
    /// The stuff from decorative plugins
    foreach(DecoratePlugin* plugin, pluginManager()->decoratePlugins){
        if(!plugin->isApplicable(document()->selectedModel())) continue;
        QAction* action = plugin->action();
        action->setCheckable(true);
        /// Add to the group (for easy event mngmnt)
        decoratorGroup->addAction( action );
        /// Insert in menus/toolbars
        menu()->insertAction(NULL,action);
        if(!action->icon().isNull())
            toolbar()->insertAction(NULL,action);
    }
    
#if 0
    /// Mark active decorators menu entries as "checked"
    foreach(DecoratePlugin* p, m->decorators)
        p->action()->setChecked(true);
#endif
    
    connect(decoratorGroup, SIGNAL(triggered(QAction*)), this, SLOT(toggleDecorator(QAction*)) );
}

void gui_selection::toggleDecorator(QAction* action){
    qDebug() << "TODO: gui_selection::toggleDecorator()" << action->isChecked();
#ifdef TODO_REENABLE_DECORATORS
    DecoratePlugin* dplugin = qobject_cast<DecoratePlugin*>(action->parent());
    Q_ASSERT(dplugin);
    Model* model = document()->selectedModel();
    
    bool removed = model->decorators.removeOne(dplugin);
    if(!removed) model->decorators.append(dplugin);

    /// Refresh the window
    mainWindow()->updateDrawArea();    
#endif
}

void gui_selection::triggerSetDefaultRenderer(){
    // qDebug() << "gui_selection::triggerSetDefaultRenderer()";
    RenderPlugin* renderer = drawArea()->activeRenderer(document()->selectedModel());
    pluginManager()->setPreferredRenderer(document()->selectedModel(),renderer);
    QString message;
    message.sprintf("Preferred renderer for '%s' set to '%s'",
                    qPrintable(document()->selectedModel()->metaObject()->className()),
                    qPrintable(renderer->name()));
    mainWindow()->statusBarMessage(message);
}

void gui_selection::triggerRenderModeAction(QAction* action){
    // qDebug("gui_selection::triggerRenderModeAction(\"%s\")",qPrintable(action->text()));
    drawArea()->setRenderer(document()->selectedModel(),action->text());
}

Q_EXPORT_PLUGIN(gui_selection)
