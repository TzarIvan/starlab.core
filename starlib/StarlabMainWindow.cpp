#include "StarlabMainWindow.h"

#include <QGLWidget>
#include <QFileOpenEvent>
#include <QToolBar>
#include <QDesktopWidget>
#include <QUrl> /// Drag&Drop
#include <QApplication>
#include <QToolTip>

#include "GuiPlugin.h"
#include "StarlabDrawArea.h"

using namespace Starlab;

StarlabMainWindow::StarlabMainWindow(StarlabApplication* _application) :
    _application(_application)
{   
    /// Init
    _modePlugin = NULL;
    _isModePluginSuspended = false;
    
    /// Setup central widget
    {
        // Anti-aliasing when using QGLWidget or subclasses
        QGLFormat glf = QGLFormat::defaultFormat();
        glf.setSamples(8);
        QGLFormat::setDefaultFormat(glf);

        _drawArea = new StarlabDrawArea(this);
        this->setCentralWidget(_drawArea);
        _drawArea->setAcceptDrops(true);
    }
    
    /// Register all plugins with the main window
    foreach(StarlabPlugin* plugin, pluginManager()->plugins()){
        plugin->_mainWindow = this;
    }
    
    
    /// Sets window icon/name
    {
        /// Put a little icon in the window title
        // QIcon icon;
        // icon.addPixmap(QPixmap(":images/___.png"));
        // setWindowIcon(icon);
        setWindowTitle("Starlab v1.0 (alpha)");        
    }
    
    /// On changes to the document, refresh the GUI
    connect(document(), SIGNAL(hasChanged()), this, SLOT(update()));
    
    /// Instantiate Menus (plugins will fill them in)
    /// Do not use the silly & windows notation for alt navigation
    {
        menus << (fileMenu      = menuBar()->addMenu("File"));
        menus << (modeMenu      = menuBar()->addMenu("Mode"));
        menus << (filterMenu    = menuBar()->addMenu("Filters"));
        menus << (renderMenu    = menuBar()->addMenu("Render"));
        menus << (decorateMenu  = menuBar()->addMenu("Decorate"));
        menus << (viewMenu      = menuBar()->addMenu("View"));
        menus << (windowsMenu   = menuBar()->addMenu("Windows"));
        menus << (helpMenu      = menuBar()->addMenu("Help"));

/// @todo this was annoying and caused bugs, temporarily disabled
#ifdef TODO_SHOW_ACTION_TOOLTIP
        /// Setup tooltips (mouse hover) for menu entries
        foreach(QMenu* menu, menus)
            connect(menu,SIGNAL(hovered(QAction*)),this,SLOT(showActionTooltip(QAction*)));
#endif
    }
    
    /// Instantiate Toolbars
    {
        mainToolbar = addToolBar(tr("Main Toolbar"));
        modeToolbar = addToolBar(tr("Edit Toolbar"));
        renderToolbar = addToolBar(tr("Render Toolbar"));
        decorateToolbar = addToolBar(tr("Decorate Toolbar"));
        filterToolbar = addToolBar(tr("Filter Toolbar"));

        /// Initially hide toolbars
        mainToolbar->setVisible(true);
        modeToolbar->setVisible(false);
        renderToolbar->setVisible(false);
        decorateToolbar->setVisible(false);
        filterToolbar->setVisible(false);
                
        /// @todo is there a better way to retrieve all sub-toolbars?
        toolbars << mainToolbar << modeToolbar << renderToolbar << decorateToolbar << filterToolbar;        
    }
    
    /// Sets up progress/status bar    
    {
        _progressBar = new QProgressBar(this);
        _statusBar   = new QStatusBar(this);
        this->setStatusBar(_statusBar);
        _progressBar->setMaximum(100);
        _progressBar->setMinimum(0);
        _progressBar->setVisible(false);
        _statusBar->addPermanentWidget(_progressBar,0);
        /// By default bottom statusbar is invisible
        _statusBar->setVisible(false);       
        
        /// @brief Whenever there is nothing to show, hide the toolbar.
        /// @todo add a timer that disables it after a while otherwise it's kind of annoying
        connect(_statusBar, SIGNAL(messageChanged(QString)), this, SLOT(hideToolbarOnEmptyMessage(QString)) );
        
        /// Show visual notification for changes in selection
        connect(document(), SIGNAL(selectionChanged(Model*)), this, SLOT(selectionChanged(Model*)));
    }
    
    /// Intercepts the "open" events sent directly by the Operative System in this->eventFilter
    {
        setAcceptDrops(true);
    }
       
    /// Connect document changes to view changes
    {
        connect(document(),SIGNAL(resetViewport()), drawArea(),SLOT(resetViewport()));
        connect(document(),SIGNAL(resetViewport()), drawArea(),SLOT(updateGL()));
    }
    
    /// Installs all the GUI plugins
    {
        foreach(GuiPlugin* plugin, pluginManager()->guiPlugins){
            plugin->_mainWindow=this;
            plugin->load();
        }
    }
        
    /// Make sure settings are fresh & inform user on where settings are loaded from
    {
        settings()->sync();
        setStatusBarMessage("Settings loaded from: "+settings()->settingsFilePath(),2);
    }
}

void StarlabMainWindow::closeEvent(QCloseEvent*){
    QApplication::exit(0);
}

void StarlabMainWindow::dropEvent(QDropEvent* event) {
    // qDebug() << "StarlabMainWindow::dropEvent()";
    const QMimeData* data = event->mimeData();
    if(data->hasUrls())
        foreach(QUrl url, data->urls())
            QApplication::sendEvent(this, new QFileOpenEvent(url));    
}


#ifdef TODO_SHOW_ACTION_TOOLTIP
void StarlabMainWindow::showActionTooltip(QAction* action){
    /// @todo can we have the tooltip appear with a delay?
    QString tip = action->toolTip();
    if(!tip.isNull())
        QToolTip::showText(QCursor::pos(), tip);
}
#endif

QSize StarlabMainWindow::sizeHint() const{
    QRect geom = QApplication::desktop()->screenGeometry();
    int scrw = geom.width();
    int scrh = geom.height();   
    return QSize(scrw/2,scrh/2);
}

void StarlabMainWindow::triggerFilterByName(QString name){
    name = name.toLower();
    setStatusBarMessage("Auto-triggering filter: '" + name + "'.");
    bool isFound=false;   
    
    /// Search the "filter" menu for the proper plugin
    foreach(QAction* action, filterMenu->actions())
        if(action->text()==name){
            isFound=true;
            action->trigger();
        }
    
    if(!isFound){
        setStatusBarMessage("Filter '"+name+"' was requested but could not be found.");
    }
}

void StarlabMainWindow::setModePlugin(ModePlugin* mode){ 
    Q_ASSERT(_modePlugin==NULL); 
    _modePlugin=mode;
    emit modePluginSet(mode);
}

/// @todo Implement this function...
void StarlabMainWindow::triggerMenuActionByName(QString name){
    QAction* trigger_me = NULL;
    foreach(QMenu* menu, menus)
        foreach(QAction* action, menu->actions())
            if(action->text()==name)
                trigger_me = action;
    
    if(trigger_me){
        setStatusBarMessage("Auto-Started: '"+ name + "'",10);
        trigger_me->trigger();
    }
    else
        setStatusBarMessage("[WARNING] Auto-Started failed: '"+ name + "'",10);
}

void StarlabMainWindow::update(){
    // qDebug() << "StarlabMainWindow::update()";
    
    /// Update all the menus/toolbars
    foreach(GuiPlugin* plugin, pluginManager()->guiPlugins)
        plugin->update();
      
    /// Only display menus and toolbars if they contain something
    {        
        /// Disable empty menus...
#if 0
        foreach(QMenu* menu, menus){
            // qDebug() << menu->title() << hasactions;
            bool hasactions = menu->actions().size()>0;
            menu->menuAction()->setVisible(hasactions);
            qDebug() << "[MENU]" << menu->title() << hasactions;
        }
#endif
        
        /// Disable empty toolbars...
        foreach(QToolBar* tb, toolbars){
            bool hasactions = tb->actions().size()>0;
            tb->setVisible(hasactions);
        }
    }
}

void StarlabMainWindow::setStatusBarMessage(QString message, double timeout_seconds){
    /// Setup tooltip of old messages
    _oldMessages.prepend(message);
    QString tooltipMessage = "Message Log (first is recent):";
    foreach(QString old, _oldMessages) tooltipMessage.append("\n-> "+old);
    _statusBar->setToolTip(tooltipMessage);
    /// Show the current message
    _statusBar->showMessage(message, (int)timeout_seconds*1000);
    _statusBar->show();
    _statusBar->update();
    /// Make sure we can see something, redraw event processed
    /// http://lists.trolltech.com/qt-interest/2008-08/msg00325.html
    QApplication::processEvents();
}

void StarlabMainWindow::setProgressBarValue(double completion){
    completion = qBound(completion, 0.0, 1.0);
    // _statusBar->showMessage(actor); ///< @todo is 0 timeout appropriate?    
    _progressBar->setEnabled(true);
    _progressBar->setValue(completion*100);
    _progressBar->show();
    /// Make sure we can see something, redraw event processed
    /// http://lists.trolltech.com/qt-interest/2008-08/msg00325.html
    QApplication::processEvents();
}

void StarlabMainWindow::closeProgressBar(){
    _progressBar->hide();
    _progressBar->reset();
}

void StarlabMainWindow::hideToolbarOnEmptyMessage(QString /*message*/){
    // qDebug() << "NEW MESSAGE: " << message;
    /// @todo disabled as it's annoying to see it popping in/out
    // if(message.isEmpty())  _statusBar->hide();
}

void StarlabMainWindow::selectionChanged(Model *selected){
    this->setStatusBarMessage("Selected model has been changed to '" + selected->name + "'", 3);
}
