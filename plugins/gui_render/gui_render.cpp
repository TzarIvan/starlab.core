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
    toolbar()->clear();
    menu()->clear();

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

void gui_render::triggerSetDefaultRenderer(){
    // qDebug() << "gui_render::triggerSetDefaultRenderer()";
    RenderPlugin* renderer = drawArea()->activeRenderer(document()->selectedModel());
    pluginManager()->setPreferredRenderer(document()->selectedModel(),renderer);
    QString message;
    message.sprintf("Preferred renderer for '%s' set to '%s'",
                    qPrintable(document()->selectedModel()->metaObject()->className()),
                    qPrintable(renderer->name()));
    mainWindow()->statusBarMessage(message);
}

void gui_render::triggerRenderModeAction(QAction* action){
    // qDebug("gui_render::triggerRenderModeAction(\"%s\")",qPrintable(action->text()));
    drawArea()->setRenderer(document()->selectedModel(),action->text());
}

Q_EXPORT_PLUGIN(gui_render)
