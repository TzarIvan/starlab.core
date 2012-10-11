#pragma once
#include <QFrame>
#include <QWidget>
#include <QKeyEvent>
#include "RichParameter.h"
#include "parameters/RichParameterSet.h"
#include "RichParameterWidget.h"
#include "starlib_global.h"

/// @todo My version of the old StdParFrame
class STARLIB_EXPORT ParametersFrame : public QFrame {
    Q_OBJECT

private:
    /// The parameters represented by this frame
    RichParameterSet* parameters;
    QList<RichParameterWidget*> widgets; 
    
public:
    ~ParametersFrame(){}
    ParametersFrame(QWidget* parent=NULL, QWidget* /*gla*/=NULL){
        this->setParent(parent);
        parameters = NULL;
        if(layout()) delete layout();
        QGridLayout * vLayout = new QGridLayout(this);
        vLayout->setAlignment(Qt::AlignTop);
        // Show only when loading frame content
        this->hide();
    }
    
    void load(RichParameterSet* parameters);
    void readValues();
    void toggleHelp(){}
   
signals:
    void dynamicFloatChanged(int mask);
    void parameterChanged();
};
