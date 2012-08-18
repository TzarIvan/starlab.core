#include "gui_view.h"

void gui_view::load(){
    /// @todo re-enable them
    load_selectdrawarea();
    // load_viewfrom();
    // load_fullscreen();
    // load_copypasteviewmatrix();
    
    { /// @todo
        // toolBarMenu = viewMenu->addMenu(tr("&ToolBars"));
        // toolBarMenu->addAction(showToolbarStandardAct);
        // connect(toolBarMenu,SIGNAL(aboutToShow()),this,SLOT(updateMenus()));
    }
}

void gui_view::load_copypasteviewmatrix(){
    QMenu* menu = mainWindow()->viewMenu->addMenu("View Matrix");
    {
        QAction* action = new QAction (tr("Copy View Matrix"), this);
        action->setShortcut(QKeySequence::Copy);
        connect(action, SIGNAL(triggered()), drawArea(), SLOT(viewToClipboard()));
        menu->addAction(action);
    }
    {
        QAction* action = new QAction (tr("Paste View Matrix"), this);
        action->setShortcut(QKeySequence::Paste);
        connect(action, SIGNAL(triggered()), drawArea(), SLOT(viewFromClipboard()));
        menu->addAction(action);
    }
}

void gui_view::load_selectdrawarea(){
    QMenu* menu = mainWindow()->viewMenu->addMenu("DrawArea");
    QActionGroup* group =  new QActionGroup(this);
    group->setExclusive(true);
    foreach(DrawAreaPlugin* plugin, mainWindow()->pluginManager()->drawAreaPlugins)
        menu->addAction( new QAction(plugin->name(),group) );
    connect(group, SIGNAL(triggered(QAction *)), this, SLOT(selectDrawArea(QAction *)));
    menu->setEnabled(true);
}
    
void gui_view::load_fullscreen(){
    QAction* action = new QAction (tr("&Toggle FullScreen"), this);
    action->setCheckable(true);
    action->setShortcutContext(Qt::ApplicationShortcut);
    action->setShortcut(Qt::ALT+Qt::Key_Return);
    connect(action, SIGNAL(triggered(bool)), mainWindow(), SLOT(toggle_fullscreen(bool)));            
    mainWindow()->viewMenu->addAction(action);
}

void gui_view::load_viewfrom(){
    QMenu* viewFromMenu = mainWindow()->viewMenu->addMenu("View From");

    QAction* reset = new QAction (tr("Reset"), this);
    reset->setShortcutContext(Qt::ApplicationShortcut);
    reset->setShortcut(Qt::CTRL+Qt::SHIFT+Qt::Key_H);
    connect(reset, SIGNAL(triggered()), drawArea(), SLOT(viewFrom()));
    viewFromMenu->addAction(reset);

    QActionGroup* group =  new QActionGroup(this);
    group->setExclusive(true);
    viewFromMenu->addAction( new QAction(tr("Top"),group) );
    viewFromMenu->addAction( new QAction(tr("Bottom"),group) );
    viewFromMenu->addAction( new QAction(tr("Left"),group) );
    viewFromMenu->addAction( new QAction(tr("Right"),group) );
    viewFromMenu->addAction( new QAction(tr("Front"),group) );
    viewFromMenu->addAction( new QAction(tr("Back"),group) );
    connect(group, SIGNAL(triggered(QAction *)), drawArea(), SLOT(viewFrom(QAction *)));

    viewFromMenu->setEnabled(true);
}

void gui_view::selectDrawArea(QAction* action){
    Q_ASSERT(action!=NULL);
    QString pluginName = action->text();
    DrawAreaPlugin* drawAreaPlugin = pluginManager()->drawAreaPlugins.value(pluginName,NULL);
    if(!drawAreaPlugin) throw StarlabException("Requested drawArea not found"); 
    mainWindow()->setupDrawArea(drawAreaPlugin);
    
    /// Store in the setting as default value
    settings()->set("DefaultDrawAreaPlugin",pluginName);
}

Q_EXPORT_PLUGIN(gui_view)

//void MainWindow::fullScreen() {
//    if(!isFullScreen()) {
//        toolbarState = saveState();
//        menuBar()->hide();
//        mainToolBar->hide();
//        globalStatusBar()->hide();
//        setWindowState(windowState()^Qt::WindowFullScreen);
//    } else {
//        menuBar()->show();
//        restoreState(toolbarState);
//        globalStatusBar()->show();
//        setWindowState(windowState()^ Qt::WindowFullScreen);
//        fullScreenAct->setChecked(false);
//    }
//}
