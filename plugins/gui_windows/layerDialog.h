#pragma once

#include <QtGui>
#include "Model.h"
#include "RichParameterSet.h"
#include "StarlabMainWindow.h"

class GLArea;
class MainWindow;
class QTreeWidget;
class GLLogStream;

#include <QDialog>
namespace Ui{ class layerDialog; }

/// @todo consider transforming this into yet another "Edit" plugin
class LayerDialog : public QDockWidget{
    Q_OBJECT
private:
    Ui::layerDialog* ui;
    StarlabMainWindow* mw;
    
public:
    ~LayerDialog();
    LayerDialog(StarlabMainWindow* mainwindow = 0);
    void updateDecoratorParsView();

public slots:
    /// Fills/Updates the layer table with content
    void updateTable();
    /// Restores the size of columns according to their content 
    /// @todo consider removing the parameter for this reason it might end up being NULL at times.
    void adaptLayout(QTreeWidgetItem * item);
    
    void modelItemClicked(QTreeWidgetItem * , int );
    void showEvent( QShowEvent * event );
    void showContextMenu(const QPoint& pos);
private slots:
    void on_moveModelUp_released();
    void on_moveModelDown_released();
};
