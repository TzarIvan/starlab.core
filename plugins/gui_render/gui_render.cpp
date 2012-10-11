#include "gui_render.h"
#include "StarlabDrawArea.h"

/// Since we depend on the selected model, the load is minimal
void gui_render::load(){
    // qDebug() << "gui_render::load()";
    this->renderModeGroup = new QActionGroup(this);
    this->renderModeGroup->setExclusive(true);
    this->currentAsDefault = new QAction("Set current as default...",NULL);
}

void gui_render::update(){
    // qDebug() << "gui_render::update()";
    toolbar()->clear();
    menu()->clear();

    /// Fetch current renderer
    Model* selectedModel = document()->selectedModel();
    if(selectedModel==NULL) return;
    RenderPlugin* currentRenderer = selectedModel->renderer();
    
    /// Add render modes menu/toolbar entries
    foreach(RenderPlugin* plugin, pluginManager()->getApplicableRenderPlugins(selectedModel)){
        QAction* action = plugin->action();
        action->setCheckable(true);
        
        /// "Check" an icon
        if(currentRenderer!=NULL)
            if(currentRenderer->name() == plugin->name())
                action->setChecked("true");
        
        renderModeGroup->addAction(action);
        /// If it has icon.. add it to toolbar
        if(!action->icon().isNull())
            toolbar()->addAction(action);
    }
        
    /// @internal menu can be added only after it has been filled :(
    menu()->addAction(currentAsDefault);
    menu()->addSeparator();
    menu()->addActions(renderModeGroup->actions());
    menu()->addSeparator();
    
    /// Connect click events to change in renderer system
    connect(renderModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(triggerRenderModeAction(QAction*)), Qt::UniqueConnection);
    connect(currentAsDefault, SIGNAL(triggered()), this, SLOT(triggerSetDefaultRenderer()));
}

void gui_render::triggerSetDefaultRenderer(){
    // qDebug() << "gui_render::triggerSetDefaultRenderer()";
    RenderPlugin* renderer = document()->selectedModel()->renderer();
    pluginManager()->setPreferredRenderer(document()->selectedModel(),renderer);
    QString message = QString("Preferred renderer for \"%1\" set to \"2\"")
                              .arg(document()->selectedModel()->metaObject()->className())
                              .arg(renderer->name());
    mainWindow()->statusBarMessage(message);
}

void gui_render::triggerRenderModeAction(QAction* action){
    // qDebug("gui_render::triggerRenderModeAction(\"%s\")",qPrintable(action->text()));
    Model* model = document()->selectedModel();
    document()->pushBusy();
        RenderPlugin* plugin = pluginManager()->newRenderPlugin(action->text());
        model->setRenderer(plugin);
    document()->popBusy();
}

Q_EXPORT_PLUGIN(gui_render)
