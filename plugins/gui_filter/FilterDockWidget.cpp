#include "FilterDockWidget.h"
#include "interfaces/FilterPlugin.h"
#include "ui_FilterDockWidget.h"
#include "ParametersFrame.h"

FilterDockWidget::FilterDockWidget(FilterPlugin* filterPlugin, RichParameterSet *pars, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::FilterDockWidget)
{
    ui->setupUi(this);
    this->filterPlugin = filterPlugin;
    this->pars = pars;
    this->init(pars);
    this->adjustSize();
    this->setWindowTitle(filterPlugin->name());
    
    /// Whenever docking changed, re-adjust the sizing
    connect(this,SIGNAL(topLevelChanged(bool)), this,SLOT(adjustSize()));
    
    /// Stay on top?
    // setWindowFlags(mainWindow->windowFlags() | Qt::WindowStaysOnTopHint);
}

FilterDockWidget::~FilterDockWidget(){
    delete ui;
}

void FilterDockWidget::on_applyButton_released(){
    parFrame->readValues();
    emit execute(filterPlugin, pars);
}

void FilterDockWidget::on_defaultButton_pressed(){
    parFrame->load(pars);
}

void FilterDockWidget::adjustSize(){
    ui->verticalLayoutWidget->adjustSize();
    ui->dockWidgetContents->adjustSize();
    int w = ui->dockWidgetContents->width();
    int h = ui->dockWidgetContents->height();
    ui->dockWidgetContents->setMinimumSize(w,h);
    ui->dockWidgetContents->setMaximumSize(w,h);
    this->QWidget::adjustSize();
}

void FilterDockWidget::init(RichParameterSet *pars){
    parFrame = new ParametersFrame(this);
    parFrame->load(pars);
    ui->verticalLayout->insertWidget(1,parFrame);
}
