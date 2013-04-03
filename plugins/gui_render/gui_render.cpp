#include "gui_render.h"
#include "StarlabDrawArea.h"
#include <QDockWidget>
#include <QColorDialog>

using namespace Starlab;

/// Since we depend on the selected model, the load is minimal
void gui_render::load(){
    // qDebug() << "gui_render::load()";
    renderModeGroup = new QActionGroup(this);
    renderModeGroup->setExclusive(true);
    currentAsDefault = new QAction("Set current as default...",this);
    editRenderSettings = new QAction("Edit renderer settings...",this);
    editModelColor = new QAction("Change model color...",this);
    
    /// When document changes, we make sure render menu/toolbars are up to date    
    connect(document(), SIGNAL(hasChanged()), this, SLOT(update()));
}

void gui_render::update(){
    // qDebug() << "gui_render::update()";
    toolbar()->clear();
    menu()->clear();

    /// Fetch current renderer
    Model* selectedModel = document()->selectedModel();
    if(selectedModel==NULL){
        toolbar()->setVisible(false);
        return;
    }
    
    Renderer* currentRenderer = selectedModel->renderer();
    
    /// Add render modes menu/toolbar entries
    foreach(RenderPlugin* plugin, pluginManager()->getApplicableRenderPlugins(selectedModel)){
        QAction* action = plugin->action();
        action->setCheckable(true);
        
        /// "Check" an icon
        if(currentRenderer!=NULL)
            if(currentRenderer->plugin() == plugin)
                action->setChecked("true");
        
        renderModeGroup->addAction(action);
        /// If it has icon.. add it to toolbar
        if(!action->icon().isNull())
            toolbar()->addAction(action);
    }

    /// Make toolbar visible if there is something to show
    toolbar()->setVisible(toolbar()->actions().size()>0);
    
    /// @internal menu can be added only after it has been filled :(
    menu()->addAction(editRenderSettings);
    menu()->addAction(currentAsDefault);
    menu()->addAction(editModelColor);
    menu()->addSeparator();
    menu()->addActions(renderModeGroup->actions());
        
    /// Disable settings link when no parameters are given
    editRenderSettings->setDisabled(currentRenderer->parameters()->isEmpty());
    
    /// Connect click events to change in renderer system
    connect(renderModeGroup, SIGNAL(triggered(QAction*)), this, SLOT(triggerRenderModeAction(QAction*)), Qt::UniqueConnection);
    connect(currentAsDefault, SIGNAL(triggered()), this, SLOT(triggerSetDefaultRenderer()), Qt::UniqueConnection);
    connect(editRenderSettings, SIGNAL(triggered()), this, SLOT(trigger_editSettings()), Qt::UniqueConnection);
    connect(editModelColor, SIGNAL(triggered()), this, SLOT(trigger_editColor()), Qt::UniqueConnection);
}

void gui_render::triggerSetDefaultRenderer(){
    // qDebug() << "gui_render::triggerSetDefaultRenderer()";
    Model* model = document()->selectedModel();    
    RenderPlugin* renderer = model->renderer()->plugin();
    pluginManager()->setPreferredRenderer(model,renderer);
    QString message = QString("Preferred renderer for \"%1\" set to \"%2\"")
                              .arg(model->metaObject()->className())
                              .arg(renderer->name());
    mainWindow()->setStatusBarMessage(message);
}

void gui_render::trigger_editSettings(){
    Renderer* renderer = document()->selectedModel()->renderer();
    /// No renderer set (btw... weird) skip
    if(renderer==NULL) return;
    /// No parameters.. avoid useless empty widget
    if(renderer->parameters()->isEmpty()) return;
    
    /// Create 
    ParametersWidget* widget = new ParametersWidget(renderer->parameters(), mainWindow());
    
    /// Add a simple title
    widget->setWindowTitle(QString("Settings for '%1'").arg(renderer->plugin()->name()));
    
    /// Behave as independent window & stay on top
    widget->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    
    /// Delete frame when its associated renderer gets killed
    connect(renderer, SIGNAL(destroyed()), widget, SLOT(deleteLater()));
    
    /// On a change in parameters, re-init, render and update
    connect(widget, SIGNAL(parameterChanged()), renderer, SLOT(init()));
    connect(widget, SIGNAL(parameterChanged()), renderer, SLOT(render()));
    connect(widget, SIGNAL(parameterChanged()), drawArea(), SLOT(updateGL()));    
    
    /// Finally show
    widget->show(); 
}

void gui_render::trigger_editColor(){
    
    QColorDialog* cd = new QColorDialog(NULL);
    cd->setOption(QColorDialog::NoButtons,true);
    /// @todo why is this not working???
    // cd->setWindowFlags(cd->windowFlags() | Qt::WindowStaysOnTopHint);
    /// Use this instead
    cd->setModal(true);
    /// on mac, native (pretty) dialog is buggy :(
    cd->setOption(QColorDialog::DontUseNativeDialog,true);
    cd->setCustomColor(0, Qt::gray); ///< Predefind model 
    cd->setCurrentColor(document()->selectedModel()->color);
    connect(cd, SIGNAL(currentColorChanged(QColor)), this, SLOT(liveColorUpdate(QColor)));
    connect(mainWindow(), SIGNAL(destroyed()), cd, SLOT(deleteLater()));
    cd->show();
}

void gui_render::liveColorUpdate(QColor color){
    // qDebug() << "color update";
    Model* model = document()->selectedModel();
    if(model==NULL) return;
    model->color = color;
    drawArea()->updateGL();        
}


void gui_render::triggerRenderModeAction(QAction* action){
    // qDebug("gui_render::triggerRenderModeAction(\"%s\")",qPrintable(action->text()));
    Model* model = document()->selectedModel();
    document()->pushBusy();
        RenderPlugin* plugin = pluginManager()->getRenderPlugin(action->text());
        model->setRenderer(plugin);
    document()->popBusy();
}

Q_EXPORT_PLUGIN(gui_render)
