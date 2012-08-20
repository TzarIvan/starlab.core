#include "StarlabMainWindow.h"

#include <QGLWidget>
#include <QFileOpenEvent>
#include <QToolBar>
#include <QDesktopWidget>
#include <QUrl> /// Drag&Drop

#include "interfaces/GuiPlugin.h"
#include "interfaces/DrawAreaPlugin.h"
#include "StarlabDrawArea.h"

/// This is the default plugin that gets loaded when no other plugin exists!
class DefaultDrawAreaPlugin : public DrawAreaPlugin{
    StarlabDrawArea* load(){ return new DummyDrawArea(mainWindow()); }

    class DummyDrawArea : public StarlabDrawArea{
    public:
        DummyDrawArea(StarlabMainWindow* mw)  : StarlabDrawArea(mw){
            _widget = new MyGLWidget();
        }
    private:
        class MyGLWidget : public QGLWidget{
        protected:
            virtual void paintGL(){ 
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
            }
        } *_widget;
    public:
        QGLWidget* widget(){ return _widget; }    
        void resetViewport(){}        
    };
};

StarlabMainWindow::StarlabMainWindow(StarlabApplication* _application) :
    _application(_application)
{
    _drawArea = NULL;
    _activeMode = NULL;
    
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
        connect(document(), SIGNAL(hasChanged()), this, SLOT(update()));
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

        /// @todo Why was this necessary in MeshLab?
        // mainToolBar->setIconSize(QSize(32,32)); 
        
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
    
    /// Install the DrawArea plugins
    {
        DrawAreaPlugin* plugin = NULL;
        if(pluginManager()->drawAreaPlugins.size()==0){
            // throw StarlabException("Critical error: Not a single DrawAreaPlugin was loaded!");
            plugin = new DefaultDrawAreaPlugin();
        } else {
            QString preferred = settings()->getString("DefaultDrawAreaPlugin");        
            if( !pluginManager()->drawAreaPlugins.contains(preferred) ){
                plugin = pluginManager()->drawAreaPlugins.begin().value();
                if(!plugin) throw StarlabException("Error setting default drawarea");
                qDebug() << "Preferred DrawAreaPlugin " << preferred << " was not found."; 
                qDebug() << "The first available plugin " << plugin->name() << " was chosen instead.";
            } else
                plugin = pluginManager()->drawAreaPlugins[preferred];            
        }
        
        /// Load and Show the Draw Window
        this->setupDrawArea( plugin );
    }
    
    /// Connect document changes to view changes
    {
        connect(document(),SIGNAL(resetViewport()), drawArea(),SLOT(resetViewport()));
        connect(document(),SIGNAL(resetViewport()), drawArea(),SLOT(update()));
    }
    
    /// Installs all the GUI plugins
    {
        foreach(GuiPlugin* plugin, pluginManager()->guiPlugins){
            plugin->_mainWindow=this;
            plugin->load();
        }
    }
    
    /// Default window size
    {
//        QDesktopWidget* desk = QApplication::desktop();
//        QRect geom = desk->screenGeometry();
//        int scrw = geom.width();
//        int scrh = geom.height();
//        geom.setTop( scrh/5);
//        geom.setLeft( scrw/4);
//        geom.setHeight( scrh/2 );
//        geom.setWidth( scrw/2 );
//        this->setGeometry(geom);
    }
    
    /// Makes the window steal the focus (@see QWidget::activateWindow())
    {                          
        showNormal();
        activateWindow();
        raise();
    }
    
    /// Delete window on close
    setAttribute(Qt::WA_DeleteOnClose, true);
    
    /// Re-fresh the window (which removes empty toolbars.. etc..)
    this->update();
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
//    geom.setTop( scrh/5);
//    geom.setLeft( scrw/4);
//    geom.setHeight( scrh/2 );
//    geom.setWidth( scrw/2 );
//    this->setGeometry(geom);
    
    return QSize(scrh/2,scrw/2);
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
    
    /// Update the drawArea
    _drawArea->update();
    
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

void StarlabMainWindow::updateDrawArea(){
    _drawArea->widget()->update();
}

void StarlabMainWindow::statusBarMessage(QString message, float timeout){
    _statusBar->show();
    _statusBar->showMessage(message, (int)timeout*1000);
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

void StarlabMainWindow::setupDrawArea(DrawAreaPlugin* plugin){
    plugin->_mainWindow = this;
    StarlabDrawArea* drawArea = plugin->load();
    Q_ASSERT( drawArea );
    if(drawArea->widget()==NULL)
        throw StarlabException("The DrawAreaPlugin you are attempting to load does not specify a widget");
    StarlabDrawArea* oldDrawArea = _drawArea;
    if(oldDrawArea != NULL) delete oldDrawArea; 
    _drawArea = drawArea;
    _drawArea->widget()->setParent(this);
    _drawArea->widget()->setAcceptDrops(true);
    /// All events of "widget" get forwarded to StarlabDrawArea
    _drawArea->widget()->installEventFilter(_drawArea); 
    this->setCentralWidget(_drawArea->widget());
    _drawArea->widget()->show();
    /// IBRAHEEM: what should we call to have the stuff 
    /// refresh immediately??    
}
