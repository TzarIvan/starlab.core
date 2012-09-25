#include "gui_decorate.h"
#include "StarlabDrawArea.h"

/// Since we depend on the selected model, the load is minimal
void gui_decorate::load(){
    // qDebug() << "gui_decorate::load()";
    this->decoratorGroup = new QActionGroup(this);
    this->decoratorGroup->setExclusive(false);
}

void gui_decorate::update(){
    toolbar()->clear();
    menu()->clear();

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

void gui_decorate::toggleDecorator(QAction* action){
    qDebug() << "TODO: gui_decorate::toggleDecorator()" << action->isChecked();
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

Q_EXPORT_PLUGIN(gui_decorate)
