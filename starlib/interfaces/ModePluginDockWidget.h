#pragma once
#include <QDockWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QMainWindow>
#include "StarlabMainWindow.h"

class ModePluginDockWidget : public QDockWidget{

/// @{ constructors: similar to QDockWidget but parent forced to be starlab main window
public:
    explicit ModePluginDockWidget(const QString &title, Starlab::MainWindow* parent = 0, Qt::WindowFlags flags = 0):
        QDockWidget(title, (QWidget*) parent, flags), _mainWindow(parent) { setup(); }
    explicit ModePluginDockWidget(Starlab::MainWindow *parent = 0, Qt::WindowFlags flags = 0):
        QDockWidget((QWidget*) parent,flags), _mainWindow(parent){ setup(); }
private:
    Starlab::MainWindow * const _mainWindow;
private:
    void setup(){
        // this->setAllowedAreas(Qt::RightDockWidgetArea);
        /// Disables closable
        this->setFeatures( (QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable) );
        
        /// Dock widgets don't have a default frame, create one
        QFrame* frame = new QFrame(this);
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment( Qt::AlignTop );
        frame->setLayout(layout);
        this->setWidget(frame);
    }
/// @}
       
/// @{ easy access properties
public:
    Starlab::Document* document(){ return _mainWindow->document(); }
    Starlab::Model* selection(){ return document()->selectedModel(); }    
/// @}
    
public:
    /// Use this to add widgets to the layout
    void addWidget(QWidget *w){
        Q_ASSERT(layout());
        Q_ASSERT(widget());
        this->widget()->layout()->addWidget(w);
    }
    /// Docks the widget on the right hand side of the main window
    void dockMe(){
        QMainWindow* mainWindow = dynamic_cast<QMainWindow*>( this->parent() );
        mainWindow->addDockWidget(Qt::RightDockWidgetArea,this);
    }
};
