#pragma once
#include "interfaces/ModePlugin.h"

class cloud_mode_selection : public ModePlugin{
    Q_OBJECT
    Q_INTERFACES(ModePlugin)
    
    QIcon icon(){ return QIcon(":/icons/select_points.png"); }   
    
    /// Functions part of the EditPlugin system
    void createEdit(){}
    void destroyEdit(){}    
    
    bool mousePressEvent(QMouseEvent*);
    bool mouseMoveEvent(QMouseEvent*);
    bool mouseReleaseEvent(QMouseEvent*);

    void decorate();
    void drawSelectionRectangle();

    bool isDragging;
    QPoint start;
    QPoint cur;
    QPoint prev;
    
};
