#include "StarlabMainWindow.h"

#include <QGLWidget>
#include <QFileOpenEvent>
#include <QToolBar>
#include <QDesktopWidget>
#include <QUrl> /// Drag&Drop

#include "interfaces/GuiPlugin.h"
#include "StarlabDrawArea.h"

StarlabMainWindow::StarlabMainWindow(StarlabApplication* _application) :
    _application(_application), _activeMode(NULL)
{   
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
    
    /// Register all plugins access functionality
    foreach(StarlabPlugin* plugin, pluginManager()->plugins())
        plugin->_mainWindow = this;
    
    
    /// Sets window icon/name
    {
        /// Put a little icon in the window title
        // QIcon icon;
        // icon.addPixmap(QPixmap(":images/___.png"));
        // setWindowIcon(icon);
        setWindowTitle("Starlab v1.0 (alpha)");        
    }
    
    /// On changes to the document, refresh the GUI
    {
        connect(document(), SIGNAL(hasChanged()), this,       SLOT(update()));
        connect(document(), SIGNAL(hasChanged()), drawArea(), SLOT(update()));
    }
        
    /// Instantiate Menus (plugins will fill them in)
    /// Do not use the silly & windows notation for alt navigation
    {
        fileMenu = menuBar()->addMenu(tr("File"));
        modeMenu = menuBar()->addMenu(tr("Mode"));
        filterMenu = menuBar()->addMenu(tr("Filters"));
        selectionMenu = menuBar()->addMenu(tr("Selection"));
        viewMenu = menuBar()->addMenu(tr("View"));
        windowsMenu = menuBar()->addMenu(tr("Windows"));
        helpMenu = menuBar()->addMenu(tr("Help"));
        
        // editMenu->clear();
        
        /// @todo is there a better way to retrieve all sub-menus?
        /// I attempted "foreach(QAction* menu_a, menuBar()->actions()){...}"
        menus << fileMenu << modeMenu << filterMenu 
              << selectionMenu << viewMenu << windowsMenu 
              << helpMenu;
    }
    
    /// Instantiate Toolbars
    {
        mainToolbar = addToolBar(tr("Main Toolbar"));
        modeToolbar = addToolBar(tr("Edit Toolbar"));
        selectionToolbar = addToolBar(tr("Selection Toolbar"));
        filterToolbar = addToolBar(tr("Filter Toolbar"));

        /// Initially hide toolbars
        mainToolbar->setVisible(true);
        modeToolbar->setVisible(false);
        selectionToolbar->setVisible(false);
        filterToolbar->setVisible(false);
                
        /// @todo is there a better way to retrieve all sub-toolbars?
        toolbars << mainToolbar << modeToolbar << selectionToolbar << filterToolbar;        
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
    }
    
    /// Intercepts the "open" events sent directly by the Operative System in this->eventFilter
    {
        setAcceptDrops(true);
    }
       
    /// Connect document changes to view changes
    {
        connect(document(),SIGNAL(resetViewport()), drawArea(),SLOT(resetViewport()));
    }
    
    /// Installs all the GUI plugins
    {
        foreach(GuiPlugin* plugin, pluginManager()->guiPlugins){
            plugin->_mainWindow=this;
            plugin->load();
        }
    }
    
    /// Makes the window steal the focus (@see QWidget::activateWindow())
    {                          
        showNormal();
        activateWindow();
        raise();
    }
    
    /// Delete window on close
    setAttribute(Qt::WA_DeleteOnClose, true);
    
    /// Make sure settings are fresh & inform user on where settings are loaded from
    {
        settings()->sync();
        statusBarMessage("Settings loaded from: "+settings()->settingsFilePath(),2);
    }
}

void StarlabMainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* data = event->mimeData();
    if(data->hasUrls())
        foreach(QUrl url, data->urls())
            QApplication::sendEvent(application(), new QFileOpenEvent(url));    
}

QSize StarlabMainWindow::sizeHint() const{
    QRect geom = QApplication::desktop()->screenGeometry();
    int scrw = geom.width();
    int scrh = geom.height();   
    return QSize(scrw/2,scrh/2);
}

void StarlabMainWindow::triggerFilterByName(QString name){
    name = name.toLower();
    statusBarMessage("Auto-triggering filter: '" + name + "'.");
    bool isFound=false;   
    
    /// Search the "filter" menu for the proper plugin
    foreach(QAction* action, filterMenu->actions())
        if(action->text().toLower()==name){
            isFound=true;
            action->trigger();
        }
    
    if(!isFound){
        statusBarMessage("Filter '"+name+"' was requested but could not be found.");
    }
}

/// @todo Implement this function...
void StarlabMainWindow::triggerMenuActionByName(QString name){
    qDebug() << "TODO: StarlabMainWindow::triggerMenuActionByName( " << name << ")";
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

void StarlabMainWindow::statusBarMessage(QString message, float timeout_seconds){
    /// Setup tooltip of old messages
    _oldMessages.prepend(message);
    QString tooltipMessage = "Message Log (first is recent):";
    foreach(QString old, _oldMessages) tooltipMessage.append("\n-> "+old);
    _statusBar->setToolTip(tooltipMessage);
    /// Show the current message
    _statusBar->showMessage(message, (int)timeout_seconds*1000);
    _statusBar->show();
    _statusBar->update();
}

void StarlabMainWindow::progressBarPercentage(QString actor, float completion){
    _statusBar->showMessage(actor); ///< @todo is 0 timeout appropriate?    
    _progressBar->show();
    _progressBar->setEnabled(true);
    _progressBar->setValue(completion);
}

void StarlabMainWindow::hideToolbarOnEmptyMessage(QString /*message*/){
    // qDebug() << "NEW MESSAGE: " << message;
    /// @todo disabled as it's annoying to see it popping in/out
    // if(message.isEmpty())  _statusBar->hide();
}
