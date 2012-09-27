#pragma once
#include "CloudPlugins.h"
#include "StarlabMainWindow.h"

class cloud_mode_select : public CloudModePlugin{
    Q_OBJECT
    Q_INTERFACES(ModePlugin)

    QIcon icon(){ return QIcon(":/icons/select_points.png"); }
    
    /// Functions part of the EditPlugin system
    void create(){}
    void destroy(){}
    
    bool mousePressEvent(QMouseEvent*);
    bool mouseMoveEvent(QMouseEvent*);
    bool mouseReleaseEvent(QMouseEvent*);

    void decorate();
    void drawWithNames();
    void drawSelectionRectangle();

    // Selection
    QSet<int> selection;
    enum SelectionMode{ NONE, ADD, REMOVE } selectionMode;

    void endSelection(const QPoint &);

    QPoint start;
    QPoint cur;
    QPoint prev;
};