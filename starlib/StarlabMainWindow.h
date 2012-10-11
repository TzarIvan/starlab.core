#pragma once
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QDragEnterEvent>
#include <QToolBar>

#include "StarlabException.h"
#include "StarlabSettings.h"
#include "StarlabApplication.h"
#include "Document.h"
#include "starlib_global.h"

class PluginManager;
class DrawAreaPlugin;

/// The basic structure of the stalab window to which plugins (i.e. GUI plugins) interface
class STARLIB_EXPORT StarlabMainWindow : public QMainWindow{
    Q_OBJECT
    
    /// @{ Core
    public:
        StarlabMainWindow(StarlabApplication *_application);
    public: 
        StarlabApplication* application(){ return _application; }
        /// The global settings of the application
        StarlabSettings* settings(){ return application()->settings(); }
        /// Contains the document (extracted from application)
        Document* document(){ return application()->document(); }
        /// Contains the loaded plugins (extracted from application)
        PluginManager* pluginManager(){ return application()->pluginManager(); }   
    private:
        /// Contains common (non-gui) functionalities
        StarlabApplication* const _application;
    /// @}

    /// @{ Basic GUI behavior
        /// Quit whole application whenever the main window is closed
        void closeEvent(QCloseEvent *);        
    /// @}
        
    /// @{ DrawArea Management (Center of Starlab window) 
    private:
        /// Manager of main drawing area (MeshLab: GLArea)
        StarlabDrawArea* _drawArea; /// NULL
    public:
        /// Returns pointer to the active draw area
        StarlabDrawArea* drawArea(){ return _drawArea; }
        /// Setup the StarlabDrawAreas loaded by a DrawAreaPlugins
        void setupDrawArea(DrawAreaPlugin *plugin);
    /// @}
               
    /// @{ ModePlugin Management
    private:
        ModePlugin* _modePlugin;
        bool _isModePluginSuspended;
    public:
        bool isModePluginSuspended(){ Q_ASSERT(_modePlugin); return _isModePluginSuspended; }
        void resumeModePlugin(){ Q_ASSERT(_modePlugin); _isModePluginSuspended=false; }
        void suspendModePlugin(){ Q_ASSERT(_modePlugin); _isModePluginSuspended=true; }
        bool hasModePlugin(){ return (_modePlugin!=NULL); }
        void setModePlugin(ModePlugin* mode){ Q_ASSERT(_modePlugin==NULL); _modePlugin=mode; }
        void removeModePlugin(){ Q_ASSERT(_modePlugin); _modePlugin=NULL; }
        ModePlugin* getModePlugin(){ Q_ASSERT(_modePlugin); return _modePlugin; }
    /// @}
    
public:   
    /// @{ @name Window's menus (in order)
    public:
        QMenu *preferencesMenu; ///< @todo settings menu, refer to MeshLab
        QMenu *fileMenu;        ///< @todo
        QMenu *modeMenu;        ///< EditPlugin entries are stored here
        QMenu *filterMenu;      ///< FilterPlugin entries are stored here
        QMenu *renderMenu;      ///< RenderPlugin entries are stored here
        QMenu *decorateMenu;    ///< DecoratePlugin entries are stored here
        QMenu *viewMenu;        ///< StarlabDrawArea component responsible for this menu
        QMenu *windowsMenu;
        QMenu *helpMenu;
    private:
        /// @todo is there a better way to retrieve all sub-menus? I attempted "foreach(QAction* menu_a, menuBar()->actions()){...}"
        QList<QMenu*> menus;    ///< List of pointers to all the above
    /// @}

    /// @{ @name Window's toolbars (in order)
    public:
        QToolBar *mainToolbar;      ///< Core icons, always visible regardless
        QToolBar *modeToolbar;      ///< Edit-specific icons, only added by edits which....QMenu *fileMenu;           
        QToolBar *filterToolbar;    ///< Filter-specific icons, only added by filters which....
        QToolBar *renderToolbar;    ///< Toolbar icons for rendering
        QToolBar *decorateToolbar;  ///< Toolbar icons for decorations
    private:
        QList<QToolBar*> toolbars; ///< List of all available toolbars
    /// @}
        
    public:
        
    /// @brief Executes a filter by name, achieving the same effect as clicking on an action. 
    /// @todo what if two filters have the same name?
    void triggerFilterByName(QString name);
    
    
    /// @brief Executes a menu command by name, achieving the same effect as clicking on an action. 
    void triggerMenuActionByName(QString name);
        
    private slots:
    /// @brief Update the whole Starlab Window
    /// => reloads toolbar & menus
    /// @note See drawArea if that's what you want to update
    void update();
    
    /// @{ @name Window's statusbar management
    public slots:
        /// Show message on statusbar, default timeout is 2 seconds
        /// Message stays permanent with timeout_seconds set to 0
        virtual void statusBarMessage(QString message, float timeout_seconds=2.0f);
        /// Sets statusbar progress to a certain value
        /// @par Who is performing an action?
        /// @par What's the completion? [0..1]
        virtual void progressBarPercentage(QString actor, float completion);
    private slots: 
        void hideToolbarOnEmptyMessage(QString message);
    private:
        QStatusBar*      _statusBar; ///< This should not be touched, use this->setStatusBarMessage
        QProgressBar*  _progressBar; ///< This should not be touched, use this->setProgressBarPercentage
        QList<QString> _oldMessages; ///< Saves all messages sent to statusbar
    /// @}

/// @{ @name Core Events Management
public slots:
    /// Manages Drag & Drop events
    void dropEvent(QDropEvent* event);
    /// @todo precisely why?
    void dragEnterEvent(QDragEnterEvent* event){ event->accept(); }
    /// This slot receives an action and displays a tooltip for it at the current mouse position
/// @}

#ifdef TODO_SHOW_ACTION_TOOLTIP
    /// i.e. typically connected to the hovered(QAction*) signal to show tooltips
    void showActionTooltip(QAction*action);
#endif
    
public:
    /// Determines default window size
    QSize sizeHint() const;
};
 
Q_DECLARE_INTERFACE(StarlabMainWindow, "starlab.StarlabMainWindow/1.0")