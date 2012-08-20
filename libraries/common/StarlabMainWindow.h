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
#include "dynamic_common_global.h"

class PluginManager;
class DrawAreaPlugin;

/// The basic structure of the stalab window to which plugins (i.e. GUI plugins) interface
class DYNAMIC_COMMON_EXPORT StarlabMainWindow : public QMainWindow{
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
               
    /// @{ edit plugin management
    private:
        ModePlugin* _activeMode;
    public:
        bool hasActiveMode(){ return _activeMode!=NULL; }
        void setActiveMode(ModePlugin* mode){ _activeMode=mode; }
        ModePlugin* activeMode(){ return _activeMode; }
    /// @}
    
public:   
    /// @{ @name Window's menus (in order)
    public:
        QMenu *preferencesMenu; ///< @todo settings menu, refer to MeshLab
        QMenu *fileMenu;        ///< @todo
        QMenu *modeMenu;        ///< EditPlugin entries are stored here
        QMenu *filterMenu;      ///< FilterPlugin entries are stored here
        QMenu *selectionMenu;   ///< Dynamically updated according to selection
        QMenu *viewMenu;           
        QMenu *windowsMenu;
        QMenu *helpMenu;
    private:
        QList<QMenu*> menus;    ///< List of pointers to all the above
    /// @}

    /// @{ @name Window's toolbars (in order)
    public:
        QToolBar *mainToolbar;      ///< Core icons, always visible regardless
        QToolBar *modeToolbar;      ///< Edit-specific icons, only added by edits which....QMenu *fileMenu;           
        QToolBar *filterToolbar;    ///< Filter-specific icons, only added by filters which....
        QToolBar *selectionToolbar; ///< Dynamically updated according to selection
    private:
        QList<QToolBar*> toolbars; ///< List of all available toolbars
    /// @}
        
    public:
        
    /// @brief Executes a filter by name, achieving the same effect as clicking on an action. 
    /// @todo what if two filters have the same name?
    void triggerFilterByName(QString name);
    
    
    /// @brief Executes a menu command by name, achieving the same effect as clicking on an action. 
    void triggerMenuActionByName(QString name);
        
    public slots:
    /// @brief Update the whole Starlab Window
    /// => reloads menus
    /// => refreshes renderer
    /// => ... missing something?
    virtual void update();
    void updateDrawArea();
    
    /// @{ @name Window's statusbar management
    public slots:
        /// Sets statusbar message to something
        virtual void statusBarMessage(QString message, float timeout=0.0f);
        /// Sets statusbar progress to a certain value
        /// @par Who is performing an action?
        /// @par What's the completion? [0..1]
        virtual void progressBarPercentage(QString actor, float completion);
    private slots: 
        void hideToolbarOnEmptyMessage(QString message);
    private:
        QStatusBar*   _statusBar;   ///< This should not be touched, use this->setStatusBarMessage
        QProgressBar* _progressBar; ///< This should not be touched, use this->setProgressBarPercentage
    /// @}

    /// @{ @name Core Events Management
    public slots:
        /// Manages Drag & Drop events
        virtual void dropEvent(QDropEvent* event);
        /// @todo precisely why?
        virtual void dragEnterEvent(QDragEnterEvent* event){ event->accept(); }
    /// @}
        
    /// Determines default window size
    QSize sizeHint() const;
};
 
Q_DECLARE_INTERFACE(StarlabMainWindow, "starlab.StarlabMainWindow/1.0")
