#include "stdpardialog.h"

#include <QtGui>
#include "StarlabException.h"

using namespace vcg;
MeshlabStdDialog::MeshlabStdDialog(QWidget *p) : QDockWidget(QString("Plugin"), p) {

    assert(false); /// @deprecated
    qf = NULL;
    stdParFrame=NULL;
    clearValues();
}

StdParFrame::StdParFrame(QWidget *p, QWidget *curr_gla ) : QFrame(p) {
    gla=curr_gla;
}

bool MeshlabStdDialog::isDynamic() {
    /// @todo
    return false;
}

void MeshlabStdDialog::clearValues() {
    curAction = NULL;
    curModel = NULL;
    curmfi = NULL;
    curmwi = NULL;
}

void MeshlabStdDialog::createFrame() {
    if(qf) delete qf;

    QFrame *newqf= new QFrame(this);
    setWidget(newqf);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    qf = newqf;
}

// update the values of the widgets with the values in the paramlist;
void MeshlabStdDialog::resetValues() {
    curParSet.clear();
#ifdef OLDMESHLAB
    curmfi->initParameterSet(*curMeshDoc, curParSet);
#endif

    assert(qf);
    assert(qf->isVisible());
// assert(curParSet.paramList.count() == stdfieldwidgets.count());
    stdParFrame->resetValues(curParSet);
}

void StdParFrame::resetValues(RichParameterSet &curParSet) {
    QList<RichParameter*> &parList =curParSet.paramList;
    assert(stdfieldwidgets.size() == parList.size());
    for(int i = 0; i < parList.count(); i++) {
        RichParameter* fpi= parList.at(i);
        if (fpi != NULL)
            stdfieldwidgets[i]->resetValue();
    }
}

/* creates widgets for the standard parameters */

void MeshlabStdDialog::loadFrameContent(Document *mdPt) {
    assert(qf);
    qf->hide();
    QLabel *ql;

    QGridLayout *gridLayout = new QGridLayout(qf);
    qf->setLayout(gridLayout);

    setWindowTitle(curmfi->name());
    ql = new QLabel("<i>"+curmfi->description()+"</i>",qf);
    ql->setTextFormat(Qt::RichText);
    ql->setWordWrap(true);
    gridLayout->addWidget(ql,0,0,1,2,Qt::AlignTop); // this widgets spans over two columns.

    stdParFrame = new StdParFrame(this,curgla);
    stdParFrame->loadFrameContent(curParSet,mdPt);
    gridLayout->addWidget(stdParFrame,1,0,1,2);

    int buttonRow = 2;  // the row where the line of buttons start

    QPushButton *helpButton = new QPushButton("Help", qf);
    QPushButton *closeButton = new QPushButton("Close", qf);
    QPushButton *applyButton = new QPushButton("Apply", qf);
    QPushButton *defaultButton = new QPushButton("Default", qf);

#ifdef Q_WS_MAC
    // Hack needed on mac for correct sizes of button in the bottom of the dialog.
    helpButton->setMinimumSize(100, 25);
    closeButton->setMinimumSize(100,25);
    applyButton->setMinimumSize(100,25);
    defaultButton->setMinimumSize(100, 25);
#endif

    if(isDynamic()) {
        previewCB = new QCheckBox("Preview", qf);
        previewCB->setCheckState(Qt::Unchecked);
        gridLayout->addWidget(previewCB,    buttonRow+0,0,Qt::AlignBottom);
        connect(previewCB,SIGNAL(toggled(bool)),this,SLOT( togglePreview() ));
        buttonRow++;
    }

    gridLayout->addWidget(helpButton,   buttonRow+0,1,Qt::AlignBottom);
    gridLayout->addWidget(defaultButton,buttonRow+0,0,Qt::AlignBottom);
    gridLayout->addWidget(closeButton,  buttonRow+1,0,Qt::AlignBottom);
    gridLayout->addWidget(applyButton,  buttonRow+1,1,Qt::AlignBottom);


    connect(helpButton,SIGNAL(clicked()),this,SLOT(toggleHelp()));
    connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));
    connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClick()));
    connect(defaultButton,SIGNAL(clicked()),this,SLOT(resetValues()));

    qf->showNormal();
    qf->adjustSize();

    //set the minimum size so it will shrink down to the right size	after the help is toggled
    this->setMinimumSize(qf->sizeHint());
    this->showNormal();
    this->adjustSize();
}

//void StdParFrame::loadFrameContent(ParameterDeclarationSet &curParSet,MeshDocument *mdPt)
void StdParFrame::loadFrameContent(RichParameterSet& curParSet, Document* /*_mdPt*/ ) {

    if(layout()) delete layout();
    QGridLayout * vLayout = new QGridLayout(this);
    vLayout->setAlignment(Qt::AlignTop);
    //QLabel* lb = new QLabel("",this);
    //vLayout->addWidget(lb);

    setLayout(vLayout);

    //QLabel *ql;

    QString descr;
    RichWidgetInterfaceConstructor rwc(this);
    for(int i = 0; i < curParSet.paramList.count(); i++) {
        RichParameter* fpi=curParSet.paramList.at(i);
        fpi->accept(rwc);
        //vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
        stdfieldwidgets.push_back(rwc.lastCreated);
        helpList.push_back(rwc.lastCreated->helpLab);
    } // end for each parameter

    this->setMinimumSize(vLayout->sizeHint());
    this->showNormal();
    this->adjustSize();
}

/// This is used by the settings dialog
void StdParFrame::loadFrameContent( RichParameter* par, Document */*mdPt*/ /*= 0*/ ) {
    if(layout()) delete layout();
    QGridLayout * vLayout = new QGridLayout(this);
    vLayout->setAlignment(Qt::AlignTop);
    setLayout(vLayout);

    QString descr;
    RichWidgetInterfaceConstructor rwc(this);

    par->accept(rwc);
    //vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
    stdfieldwidgets.push_back(rwc.lastCreated);
    helpList.push_back(rwc.lastCreated->helpLab);
    showNormal();
    adjustSize();
}
void StdParFrame::toggleHelp() {
    for(int i = 0; i < helpList.count(); i++)
        helpList.at(i)->setVisible(!helpList.at(i)->isVisible()) ;
    updateGeometry();
    adjustSize();
}

void MeshlabStdDialog::toggleHelp() {
    stdParFrame->toggleHelp();
    qf->updateGeometry();
    qf->adjustSize();
    this->updateGeometry();
    this->adjustSize();
}

//void StdParFrame::readValues(ParameterDeclarationSet &curParSet)
void StdParFrame::readValues(RichParameterSet &curParSet) {
    QList<RichParameter*> &parList =curParSet.paramList;
    assert(parList.size() == stdfieldwidgets.size());
    QVector<MeshLabWidget*>::iterator it = stdfieldwidgets.begin();
    for(int i = 0; i < parList.count(); i++) {
        QString sname = parList.at(i)->name;
        curParSet.setValue(sname,(*it)->getWidgetValue());
        ++it;
    }
}

StdParFrame::~StdParFrame() {

}

/* click event for the apply button of the standard plugin window */
// If the filter has some dynamic parameters
// - before applying the filter restore the original state of the mesh.
// - after applying the filter reget the state of the mesh.

void MeshlabStdDialog::applyClick() {
#if 0
    QAction *q = curAction;
    stdParFrame->readValues(curParSet);

    ////int mask = 0;//curParSet.getDynamicFloatMask();
    if(curmask)
        meshState.apply(curModel);

    //PreView Caching: if the apply parameters are the same to those used in the preview mode
    //we don't need to reapply the filter to the mesh
    bool isEqual = (curParSet == prevParSet);
    if ((isEqual) && (validcache))
        meshCacheState.apply(curModel);
    else
        curmwi->executeFilter(q, curParSet, false);

    if(curmask)
        meshState.create(curmask, curModel);
    if(this->curgla)
        this->curgla->update();
#endif
}

void MeshlabStdDialog::applyDynamic() {
#if 0
    if(!previewCB->isChecked())
        return;
    QAction *q = curAction;
    stdParFrame->readValues(curParSet);
    //for cache mechanism
    //needed to allocate the required memory space in prevParSet
    //it called the operator=(RichParameterSet) function defined in RichParameterSet
    prevParSet = curParSet;
    stdParFrame->readValues(prevParSet);
    // Restore the
    meshState.apply(curModel);
    curmwi->executeFilter(q, curParSet, true);
    meshCacheState.create(curmask,curModel);
    validcache = true;

    if(this->curgla)
        this->curgla->update();
#endif
}

void MeshlabStdDialog::togglePreview() {
    throw StarlabException("Preview not ready yet");
#ifdef OLDMESHLAB
    if(previewCB->isChecked()) {
        stdParFrame->readValues(curParSet);
        if (!prevParSet.isEmpty() && (validcache) && (curParSet == prevParSet))
            meshCacheState.apply(curModel);
        else
            applyDynamic();
    } else
        meshState.apply(curModel);

    curgla->update();
#endif
}

/* click event for the close button of the standard plugin window */

void MeshlabStdDialog::closeClick() {
#ifdef OLDMESHLAB
    //int mask = 0;//curParSet.getDynamicFloatMask();
    if(curmask != MeshModel::MM_UNKNOWN)
        meshState.apply(curModel);
    curmask = MeshModel::MM_UNKNOWN;
    // Perform the update only if there is Valid GLarea.
    if(this->curgla)
        this->curgla->update();
    close();
#endif
}


// click event for the standard red crossed close button in the upper right widget's corner
void MeshlabStdDialog::closeEvent(QCloseEvent * /*event*/) {
    closeClick();
}

MeshlabStdDialog::~MeshlabStdDialog() {
    delete stdParFrame;
    if(isDynamic())
        delete previewCB;
}


/******************************************/
// AbsPercWidget Implementation
/******************************************/

//QGridLayout(NULL)
AbsPercWidget::AbsPercWidget(QWidget *p, RichAbsPerc* rabs):MeshLabWidget(p,rabs){
    AbsPercDecoration* absd = reinterpret_cast<AbsPercDecoration*>(rp->pd);
    m_min = absd->min;
    m_max = absd->max;

    fieldDesc = new QLabel(rp->pd->fieldDesc + " (abs and %)",p);
    fieldDesc->setToolTip(rp->pd->tooltip);
    absSB = new QDoubleSpinBox(p);
    percSB = new QDoubleSpinBox(p);

    absSB->setMinimum(m_min-(m_max-m_min));
    absSB->setMaximum(m_max*2);
    absSB->setAlignment(Qt::AlignRight);

    int decimals= 7-ceil(log10(fabs(m_max-m_min)) ) ;
    //qDebug("range is (%f %f) %f ",m_max,m_min,fabs(m_max-m_min));
    //qDebug("log range is %f ",log10(fabs(m_max-m_min)));
    absSB->setDecimals(decimals);
    absSB->setSingleStep((m_max-m_min)/100.0);
    float initVal = rp->val->getAbsPerc();
    absSB->setValue(initVal);

    percSB->setMinimum(-200);
    percSB->setMaximum(200);
    percSB->setAlignment(Qt::AlignRight);
    percSB->setSingleStep(0.5);
    percSB->setValue((100*(initVal - m_min))/(m_max - m_min));
    percSB->setDecimals(3);
    QLabel *absLab=new QLabel("<i> <small> world unit</small></i>");
    QLabel *percLab=new QLabel("<i> <small> perc on"+QString("(%1 .. %2)").arg(m_min).arg(m_max)+"</small></i>");

    gridLay->addWidget(fieldDesc,row,0,Qt::AlignHCenter);

    QGridLayout* lay = new QGridLayout(p);
    lay->addWidget(absLab,0,0,Qt::AlignHCenter);
    lay->addWidget(percLab,0,1,Qt::AlignHCenter);

    lay->addWidget(absSB,1,0,Qt::AlignTop);
    lay->addWidget(percSB,1,1,Qt::AlignTop);

    gridLay->addLayout(lay,row,1,Qt::AlignTop);

    connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
    connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

AbsPercWidget::~AbsPercWidget() {
    delete absSB;
    delete percSB;
    delete fieldDesc;
}


void AbsPercWidget::on_absSB_valueChanged(double newv) {
    percSB->setValue((100*(newv - m_min))/(m_max - m_min));
    emit dialogParamChanged();
}

void AbsPercWidget::on_percSB_valueChanged(double newv) {
    absSB->setValue((m_max - m_min)*0.01*newv + m_min);
    emit dialogParamChanged();
}

//float AbsPercWidget::getValue()
//{
//	return float(absSB->value());
//}

void AbsPercWidget::setValue(float val, float minV, float maxV) {
    assert(absSB);
    absSB->setValue (val);
    m_min=minV;
    m_max=maxV;
}

void AbsPercWidget::collectWidgetValue() {
    rp->val->set(AbsPercValue(float(absSB->value())));
}

void AbsPercWidget::resetWidgetValue() {
    const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
    setValue(rp->pd->defVal->getAbsPerc(),absd->min,absd->max);
}

void AbsPercWidget::setWidgetValue( const Value& nv ) {
    const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
    setValue(nv.getAbsPerc(),absd->min,absd->max);
}

/******************************************/
// Point3fWidget Implementation
/******************************************/


//QHBoxLayout(NULL)
Point3fWidget::Point3fWidget(QWidget *p, RichPoint3f* rpf, QWidget *gla_curr): MeshLabWidget(p,rpf) {

    paramName = rpf->name;
    //int row = gridLay->rowCount() - 1;

    descLab = new QLabel(rpf->pd->fieldDesc,p);
    descLab->setToolTip(rpf->pd->fieldDesc);
    gridLay->addWidget(descLab,row,0,Qt::AlignTop);

    QHBoxLayout* lay = new QHBoxLayout(p);

    for(int i =0; i<3; ++i) {
        coordSB[i]= new QLineEdit(p);
        QFont baseFont=coordSB[i]->font();
        if(baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize()*3/4);
        else baseFont.setPointSize(baseFont.pointSize()*3/4);
        coordSB[i]->setFont(baseFont);
        //coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width()/4);
        coordSB[i]->setMinimumWidth(0);
        coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width()/2);
        //coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
        coordSB[i]->setValidator(new QDoubleValidator(p));
        coordSB[i]->setAlignment(Qt::AlignRight);
        //this->addWidget(coordSB[i],1,Qt::AlignHCenter);
        lay->addWidget(coordSB[i]);
    }
    this->setValue(paramName,rp->val->getPoint3f());
    if(gla_curr) { // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
        getPoint3Button = new QPushButton("Get",p);
        getPoint3Button->setMaximumWidth(getPoint3Button->sizeHint().width()/2);

        getPoint3Button->setFlat(true);
        //getPoint3Button->setMinimumWidth(getPoint3Button->sizeHint().width());
        //this->addWidget(getPoint3Button,0,Qt::AlignHCenter);
        lay->addWidget(getPoint3Button);
        QStringList names;
        names << "View Dir";
        names << "View Pos";
        names << "Surf. Pos";
        names << "Camera Pos";

        getPoint3Combo = new QComboBox(p);
        getPoint3Combo->addItems(names);
        //getPoint3Combo->setMinimumWidth(getPoint3Combo->sizeHint().width());
        //this->addWidget(getPoint3Combo,0,Qt::AlignHCenter);
        lay->addWidget(getPoint3Combo);

        connect(getPoint3Button,SIGNAL(clicked()),this,SLOT(getPoint()));
        connect(getPoint3Combo,SIGNAL(currentIndexChanged(int)),this,SLOT(getPoint()));
        connect(gla_curr,SIGNAL(transmitViewDir(QString,vcg::Point3f)),this,SLOT(setValue(QString,vcg::Point3f)));
        connect(gla_curr,SIGNAL(transmitShot(QString,vcg::Shotf)),this,SLOT(setShotValue(QString,vcg::Shotf)));
        connect(gla_curr,SIGNAL(transmitSurfacePos(QString,vcg::Point3f)),this,SLOT(setValue(QString,vcg::Point3f)));
        connect(this,SIGNAL(askViewDir(QString)),gla_curr,SLOT(sendViewDir(QString)));
        connect(this,SIGNAL(askViewPos(QString)),gla_curr,SLOT(sendMeshShot(QString)));
        connect(this,SIGNAL(askSurfacePos(QString)),gla_curr,SLOT(sendSurfacePos(QString)));
        connect(this,SIGNAL(askCameraPos(QString)),gla_curr,SLOT(sendCameraPos(QString)));
    }
    gridLay->addLayout(lay,row,1,Qt::AlignTop);
}

void Point3fWidget::getPoint() {
    int index = getPoint3Combo->currentIndex();
    qDebug("Got signal %i",index);
    switch(index) {
    case 0 :
        emit askViewDir(paramName);
        break;
    case 1 :
        emit askViewPos(paramName);
        break;
    case 2 :
        emit askSurfacePos(paramName);
        break;
    case 3 :
        emit askCameraPos(paramName);
        break;
    default :
        assert(0);
    }
}

Point3fWidget::~Point3fWidget() {}

void Point3fWidget::setValue(QString name,Point3f newVal) {
    if(name==paramName) {
        for(int i =0; i<3; ++i)
            coordSB[i]->setText(QString::number(newVal[i],'g',4));
    }
}

void Point3fWidget::setShotValue(QString name,Shotf newValShot) {
    Point3f p = newValShot.GetViewPoint();
    setValue(name,p);
}

vcg::Point3f Point3fWidget::getValue() {
    return Point3f(coordSB[0]->text().toFloat(),coordSB[1]->text().toFloat(),coordSB[2]->text().toFloat());
}

void Point3fWidget::collectWidgetValue() {
    rp->val->set(Point3fValue(vcg::Point3f(coordSB[0]->text().toFloat(),coordSB[1]->text().toFloat(),coordSB[2]->text().toFloat())));
}

void Point3fWidget::resetWidgetValue() {
    for(unsigned int ii = 0; ii < 3; ++ii)
        coordSB[ii]->setText(QString::number(rp->pd->defVal->getPoint3f()[ii],'g',3));
}

void Point3fWidget::setWidgetValue( const Value& nv ) {
    for(unsigned int ii = 0; ii < 3; ++ii)
        coordSB[ii]->setText(QString::number(nv.getPoint3f()[ii],'g',3));
}

/******************************************/
// Matrix44fWidget Implementation
/******************************************/


//QHBoxLayout(NULL)
Matrix44fWidget::Matrix44fWidget(QWidget *p, RichMatrix44f* rpf,  QWidget *gla_curr): MeshLabWidget(p,rpf) {

    paramName = rpf->name;
    //int row = gridLay->rowCount() - 1;

    descLab = new QLabel(rpf->pd->fieldDesc,p);
    descLab->setToolTip(rpf->pd->fieldDesc);
    gridLay->addWidget(descLab,row,0,Qt::AlignTop);

    QVBoxLayout* lay = new QVBoxLayout(p);
    QGridLayout* lay44 = new QGridLayout(p);



    for(int i =0; i<16; ++i) {
        coordSB[i]= new QLineEdit(p);
        QFont baseFont=coordSB[i]->font();
        if(baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize()*3/4);
        else baseFont.setPointSize(baseFont.pointSize()*3/4);
        coordSB[i]->setFont(baseFont);
        //coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width()/4);
        coordSB[i]->setMinimumWidth(0);
        coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width()/2);
        //coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
        coordSB[i]->setValidator(new QDoubleValidator(p));
        coordSB[i]->setAlignment(Qt::AlignRight);
        //this->addWidget(coordSB[i],1,Qt::AlignHCenter);
        lay44->addWidget(coordSB[i],i/4,i%4);

    }
    this->setValue(paramName,rp->val->getMatrix44f());

    lay->addLayout(lay44);

    QPushButton     * getMatrixButton = new QPushButton("Read from current layer");
    lay->addWidget(getMatrixButton);

    QPushButton     * pasteMatrixButton = new QPushButton("Paste from clipboard");
    lay->addWidget(pasteMatrixButton);

    gridLay->addLayout(lay,row,1,Qt::AlignTop);

    connect(gla_curr,SIGNAL(transmitMatrix(QString,vcg::Matrix44f)),this,SLOT(setValue(QString,vcg::Matrix44f)));
    connect(getMatrixButton,SIGNAL(clicked()),this,SLOT(getMatrix()));
    connect(pasteMatrixButton,SIGNAL(clicked()),this,SLOT(pasteMatrix()));
    connect(this,SIGNAL(askMeshMatrix(QString)),  gla_curr,SLOT(sendMeshMatrix(QString)));

}


Matrix44fWidget::~Matrix44fWidget() {}

void Matrix44fWidget::setValue(QString name,Matrix44f newVal) {
    if(name==paramName) {
        for(int i =0; i<16; ++i)
            coordSB[i]->setText(QString::number(newVal[i/4][i%4],'g',4));
    }
}


vcg::Matrix44f Matrix44fWidget::getValue() {
    float val[16];
    for(unsigned int i  = 0; i < 16; ++i)
        val[i] = coordSB[i]->text().toFloat();
    return Matrix44f(val);
}

void Matrix44fWidget::getMatrix() {

    emit askMeshMatrix(QString("TransformMatrix"));
}

void Matrix44fWidget::pasteMatrix() {
    QClipboard *clipboard = QApplication::clipboard();
    QString shotString = clipboard->text();
    QStringList list1 = shotString.split(" ");
    if(list1.size() < 16) return;
    int id = 0;
    for(QStringList::iterator i = list1.begin(); i != list1.end(); ++i,++id) {
        bool ok = true;
        (*i).toFloat(&ok);
        if(!ok) return;
    }
    id = 0;
    for(QStringList::iterator i = list1.begin(); i != list1.end(); ++i,++id)
        coordSB[id]->setText(*i) ;
}

void Matrix44fWidget::collectWidgetValue() {
    vcg::Matrix44f  m;
    for(unsigned int i  = 0; i < 16; ++i) m[i/4][i%4] = coordSB[i]->text().toFloat();
    rp->val->set(Matrix44fValue(m));
}

void Matrix44fWidget::resetWidgetValue() {
    vcg::Matrix44f  m;
    m.SetIdentity();
    for(unsigned int ii = 0; ii < 16; ++ii)
        coordSB[ii]->setText(QString::number(rp->pd->defVal->getMatrix44f()[ii/4][ii%4],'g',3));
}

void Matrix44fWidget::setWidgetValue( const Value& nv ) {
    for(unsigned int ii = 0; ii < 16; ++ii)
        coordSB[ii]->setText(QString::number(nv.getMatrix44f()[ii/4][ii%4],'g',3));
}

/********************/
// ShotfWidget Implementation

ShotfWidget::ShotfWidget(QWidget *p, RichShotf* rpf, QWidget *gla_curr): MeshLabWidget(p,rpf) {

    paramName = rpf->name;
    //int row = gridLay->rowCount() - 1;

    descLab = new QLabel(rpf->pd->fieldDesc,p);
    descLab->setToolTip(rpf->pd->fieldDesc);
    gridLay->addWidget(descLab,row,0,Qt::AlignTop);

    QHBoxLayout* lay = new QHBoxLayout(p);


    this->setShotValue(paramName,rp->val->getShotf());
    if(gla_curr) { // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
        getShotButton = new QPushButton("Get shot",p);
        lay->addWidget(getShotButton);

        QStringList names;
        names << "Current Trackball";
        names << "Current Mesh";
        names << "Current Raster";
        names << "From File";

        getShotCombo = new QComboBox(p);
        getShotCombo->addItems(names);
        lay->addWidget(getShotCombo);
        connect(getShotCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(getShot()));
        connect(getShotButton,SIGNAL(clicked()),this,SLOT(getShot()));
        connect(gla_curr,SIGNAL(transmitShot(QString,vcg::Shotf)),this,SLOT(setShotValue(QString,vcg::Shotf)));
        connect(this,SIGNAL(askViewerShot(QString)),gla_curr,SLOT(sendViewerShot(QString)));
        connect(this,SIGNAL(askMeshShot(QString)),  gla_curr,SLOT(sendMeshShot(QString)));
    }
    gridLay->addLayout(lay,row,1,Qt::AlignTop);
}

void ShotfWidget::getShot() {
    int index = getShotCombo->currentIndex();
    switch(index)  {
    case 0 :
        emit askViewerShot(paramName);
        break;
    case 1 :
        emit askMeshShot(paramName);
        break;
    case 2 :
        emit askRasterShot(paramName);
        break;
    case 3: {
        QString filename = QFileDialog::getOpenFileName(this, tr("Load xml camera"), "./", tr("Xml Files (*.xml)"));
        QFile qf(filename);
        QFileInfo qfInfo(filename);

        if( !qf.open(QIODevice::ReadOnly ) )
            return ;

        QDomDocument doc("XmlDocument");    //It represents the XML document
        if(!doc.setContent( &qf ))     return;
        qf.close();

        QString type = doc.doctype().name();

        //TextAlign file project
        //if(type == "RegProjectML")   loadShotFromTextAlignFile(doc);
        //View State file
        //else if(type == "ViewState") loadViewFromViewStateFile(doc);

        //qDebug("End file reading");


        // return true;
    }
    default :
        assert(0);
    }
}

ShotfWidget::~ShotfWidget() {}

void ShotfWidget::setShotValue(QString name,Shotf newVal) {
    if(name==paramName) {
        curShot=newVal;
    }
}

vcg::Shotf ShotfWidget::getValue() {
    return curShot;
}

void ShotfWidget::collectWidgetValue() {
    rp->val->set(ShotfValue(curShot));
}

void ShotfWidget::resetWidgetValue() {
    curShot = rp->pd->defVal->getShotf();
}

void ShotfWidget::setWidgetValue( const Value& nv ) {
    curShot = nv.getShotf();
}

/********************/
// ComboWidget End Implementation

ComboWidget::ComboWidget(QWidget *p, RichParameter* rpar) :MeshLabWidget(p,rpar) {
}

void ComboWidget::Init(QWidget *p,int defaultEnum, QStringList values) {
    enumLabel = new QLabel(p);
    enumLabel->setText(rp->pd->fieldDesc);
    enumCombo = new QComboBox(p);
    enumCombo->addItems(values);
    setIndex(defaultEnum);
    //int row = gridLay->rowCount() - 1;
    gridLay->addWidget(enumLabel,row,0,Qt::AlignTop);
    gridLay->addWidget(enumCombo,row,1,Qt::AlignTop);
    connect(enumCombo,SIGNAL(activated(int)),this,SIGNAL(dialogParamChanged()));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

void ComboWidget::setIndex(int newEnum) {
    enumCombo->setCurrentIndex(newEnum);
}

int ComboWidget::getIndex() {
    return enumCombo->currentIndex();
}

ComboWidget::~ComboWidget() {
    delete enumCombo;
    delete enumLabel;
}

/******************************************/
//EnumWidget Implementation
/******************************************/

EnumWidget::EnumWidget(QWidget *p, RichEnum* rpar)
    :ComboWidget(p,rpar) {
    //you MUST call it!!!!
    Init(p,rpar->val->getEnum(),reinterpret_cast<EnumDecoration*>(rpar->pd)->enumvalues);
    //assert(enumCombo != NULL);
}

int EnumWidget::getSize() {
    return enumCombo->count();
}


void EnumWidget::collectWidgetValue() {
    rp->val->set(EnumValue(enumCombo->currentIndex()));
}

void EnumWidget::resetWidgetValue() {
    //lned->setText(QString::number(rp->val->getFloat(),'g',3));
    enumCombo->setCurrentIndex(rp->pd->defVal->getEnum());
}

void EnumWidget::setWidgetValue( const Value& nv ) {
    enumCombo->setCurrentIndex(nv.getEnum());
}

GenericParamDialog::GenericParamDialog(QWidget *p, RichParameterSet *_curParSet, QString title, Document *_meshDocument) : QDialog(p) {
    stdParFrame=NULL;
    curParSet=_curParSet;
    meshDocument = _meshDocument;
    createFrame();
    if(!title.isEmpty())
        setWindowTitle(title);
}


// update the values of the widgets with the values in the paramlist;
void GenericParamDialog::resetValues() {
    stdParFrame->resetValues(*curParSet);
}

void GenericParamDialog::toggleHelp() {
    stdParFrame->toggleHelp();
    this->updateGeometry();
    this->adjustSize();
}


void GenericParamDialog::createFrame() {
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    setLayout(vboxLayout);

    stdParFrame = new StdParFrame(this);
    stdParFrame->loadFrameContent(*curParSet, meshDocument);
    layout()->addWidget(stdParFrame);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
    //add the reset button so we can get its signals
    QPushButton *resetButton = buttonBox->addButton(QDialogButtonBox::Reset);
    layout()->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    //set the minimum size so it will shrink down to the right size	after the help is toggled
    this->setMinimumSize(stdParFrame->sizeHint());
    this->showNormal();
    this->adjustSize();
}


void GenericParamDialog::getAccept() {
    stdParFrame->readValues(*curParSet);
    accept();
}

GenericParamDialog::~GenericParamDialog() {
    delete stdParFrame;
}

/******************************************/
// DynamicFloatWidget Implementation
/******************************************/


//QGridLayout(NULL)
DynamicFloatWidget::DynamicFloatWidget(QWidget *p, RichDynamicFloat* rdf):MeshLabWidget(p,rdf) {
    minVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->min;
    maxVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->max;
    valueLE = new QLineEdit(p);
    valueLE->setAlignment(Qt::AlignRight);
    valueSlider = new QSlider(Qt::Horizontal,p);
    valueSlider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    fieldDesc = new QLabel(rp->pd->fieldDesc);
    valueSlider->setMinimum(0);
    valueSlider->setMaximum(100);
    valueSlider->setValue(floatToInt(rp->val->getFloat()));
    const DynamicFloatDecoration* dfd = reinterpret_cast<const DynamicFloatDecoration*>(&(rp->pd));
    valueLE->setValidator(new QDoubleValidator (dfd->min,dfd->max, 5, valueLE));
    valueLE->setText(QString::number(rp->val->getFloat()));


    //int row = gridLay->rowCount() - 1;
    gridLay->addWidget(fieldDesc,row,0,Qt::AlignTop);

    QHBoxLayout* lay = new QHBoxLayout(p);
    lay->addWidget(valueLE,0,Qt::AlignHCenter);
    //lay->addWidget(valueSlider,0,Qt::AlignJustify);
    lay->addWidget(valueSlider,0,0);
    gridLay->addLayout(lay,row,1,Qt::AlignTop);

    connect(valueLE,SIGNAL(textChanged(const QString &)),this,SLOT(setValue()));
    connect(valueSlider,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));

}

/****************************/
Value& MeshLabWidget::getWidgetValue() {
    collectWidgetValue();
    return *(rp->val);
}

void MeshLabWidget::resetValue() {
    rp->val->set(*rp->pd->defVal);
    resetWidgetValue();
}

void MeshLabWidget::InitRichParameter(RichParameter* rpar) {
    rp = rpar;
}




/******************************************/
// ColorWidget Implementation
/******************************************/


ColorWidget::ColorWidget(QWidget *p, RichColor* newColor)
    :MeshLabWidget(p,newColor),pickcol() {

    colorLabel = new QLabel(p);
    descLabel = new QLabel(rp->pd->fieldDesc,p);
    colorButton = new QPushButton(p);
    colorButton->setAutoFillBackground(true);
    colorButton->setFlat(true);
    //const QColor cl = rp->pd->defVal->getColor();
    //resetWidgetValue();
    initWidgetValue();
    //int row = gridLay->rowCount() - 1;
    gridLay->addWidget(descLabel,row,0,Qt::AlignTop);

    QHBoxLayout* lay = new QHBoxLayout(p);
    lay->addWidget(colorLabel);
    lay->addWidget(colorButton);

    gridLay->addLayout(lay,row,1,Qt::AlignTop);
    pickcol = rp->val->getColor();
    connect(colorButton,SIGNAL(clicked()),this,SLOT(pickColor()));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

void ColorWidget::updateColorInfo(const ColorValue& newColor) {
    QColor col = newColor.getColor();
    colorLabel->setText("("+col.name()+")");
    QPalette palette(col);
    colorButton->setPalette(palette);
}

void ColorWidget::pickColor() {
    pickcol =QColorDialog::getColor(pickcol);
    if(pickcol.isValid()) {
        collectWidgetValue();
        updateColorInfo(ColorValue(pickcol));
    }
    emit dialogParamChanged();
}

void ColorWidget::initWidgetValue() {
    QColor cl = rp->val->getColor();
    pickcol = cl;
    updateColorInfo(cl);
}

void ColorWidget::resetWidgetValue() {
    QColor cl = rp->pd->defVal->getColor();
    pickcol = cl;
    updateColorInfo(cl);
}

void ColorWidget::collectWidgetValue() {
    rp->val->set(ColorValue(pickcol));
}

ColorWidget::~ColorWidget() {
    delete colorButton;
    delete colorLabel;
    delete descLabel;
}

void ColorWidget::setWidgetValue( const Value& nv ) {
    QColor cl = nv.getColor();
    pickcol = cl;
    updateColorInfo(cl);
}
/*
void GetFileNameWidget::launchGetFileNameDialog()
{

}*/

void RichParameterToQTableWidgetItemConstructor::visit( RichBool& pd ) {
    if (pd.val->getBool())
        lastCreated = new QTableWidgetItem("true"/*,lst*/);
    else
        lastCreated = new QTableWidgetItem("false"/*,lst*/);

}

void RichParameterToQTableWidgetItemConstructor::visit( RichInt& pd ) {
    lastCreated = new QTableWidgetItem(QString::number(pd.val->getInt())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichFloat& pd ) {
    lastCreated = new QTableWidgetItem(QString::number(pd.val->getFloat())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichString& pd ) {
    lastCreated = new QTableWidgetItem(pd.val->getString()/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichPoint3f& pd ) {
    vcg::Point3f pp = pd.val->getPoint3f();
    QString pst = "P3(" + QString::number(pp.X()) + "," + QString::number(pp.Y()) + "," + QString::number(pp.Z()) + ")";
    lastCreated = new QTableWidgetItem(pst/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichShotf& /*pd*/ ) {
    assert(0); ///
    lastCreated = new QTableWidgetItem(QString("TODO")/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichOpenFile& pd) {
    lastCreated = new QTableWidgetItem(pd.val->getFileName()/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichColor& pd ) {
    QPixmap pix(10,10);
    pix.fill(pd.val->getColor());
    QIcon ic(pix);
    lastCreated = new QTableWidgetItem(ic,""/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichAbsPerc& pd ) {
    lastCreated = new QTableWidgetItem(QString::number(pd.val->getAbsPerc())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichEnum& pd ) {
    lastCreated = new QTableWidgetItem(QString::number(pd.val->getEnum())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichDynamicFloat& pd ) {
    lastCreated = new QTableWidgetItem(QString::number(pd.val->getDynamicFloat())/*,lst*/);
}

IOFileWidget::IOFileWidget( QWidget* p,RichParameter* rpar )
    :MeshLabWidget(p,rpar),fl() {
    filename = new QLineEdit(p);
    filename->setText(tr(""));
    browse = new QPushButton(p);
    descLab = new QLabel(rp->pd->fieldDesc,p);
    browse->setText("...");
    //const QColor cl = rp->pd->defVal->getColor();
    //resetWidgetValue();
    //int row = gridLay->rowCount() - 1;
    gridLay->addWidget(descLab,row,0,Qt::AlignTop);
    QHBoxLayout* lay = new QHBoxLayout(p);
    lay->addWidget(filename,2);
    lay->addWidget(browse);

    gridLay->addLayout(lay,row,1,Qt::AlignTop);

    connect(browse,SIGNAL(clicked()),this,SLOT(selectFile()));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

IOFileWidget::~IOFileWidget() {
    delete filename;
    delete browse;
    delete descLab;
}

void IOFileWidget::collectWidgetValue() {
    rp->val->set(FileValue(fl));
}

void IOFileWidget::resetWidgetValue() {
    QString fle = rp->pd->defVal->getFileName();
    fl = fle;
    updateFileName(fle);
}


void IOFileWidget::setWidgetValue(const Value& nv) {
    QString fle = nv.getFileName();
    fl = fle;
    updateFileName(QString());
}

void IOFileWidget::updateFileName( const FileValue& file ) {
    filename->setText(file.getFileName());
}

OpenFileWidget::OpenFileWidget( QWidget *p, RichOpenFile* rdf )
    :IOFileWidget(p,rdf) {
}

void OpenFileWidget::selectFile() {
    OpenFileDecoration* dec = reinterpret_cast<OpenFileDecoration*>(rp->pd);
    QString ext;
    qDebug() << "RIGHT WHERE I WANT YOU!!";
    fl = QFileDialog::getOpenFileName(this,tr("Open"),dec->defVal->getFileName(),dec->exts.join(" "));
    collectWidgetValue();
    updateFileName(fl);
    emit dialogParamChanged();
}

//void OpenFileWidget::collectWidgetValue()
//{
//	rp->val->set(FileValue(fl));
//}
//
//void OpenFileWidget::resetWidgetValue()
//{
//	QString fle = rp->pd->defVal->getFileName();
//	fl = fle;
//	updateFileName(fle);
//}
//
//
//void OpenFileWidget::setWidgetValue(const Value& nv)
//{
//	QString fle = nv.getFileName();
//	fl = fle;
//	updateFileName(QString());
//}

OpenFileWidget::~OpenFileWidget() {
}


SaveFileWidget::SaveFileWidget( QWidget* p,RichSaveFile* rpar )
    :IOFileWidget(p,rpar) {
    filename->setText(rpar->val->getFileName());
}

SaveFileWidget::~SaveFileWidget() {
}

void SaveFileWidget::selectFile() {
    SaveFileDecoration* dec = reinterpret_cast<SaveFileDecoration*>(rp->pd);
    QString ext;
    fl = QFileDialog::getSaveFileName(this,tr("Save"),dec->defVal->getFileName(),dec->ext);
    collectWidgetValue();
    updateFileName(fl);
    emit dialogParamChanged();
}

//XMLBoolWidget::XMLBoolWidget( QWidget* parent,const QStringList& xmlWidgetTag )
//{
//
//}

//XMLMeshLabWidget::XMLMeshLabWidget( QWidget* p )
//:QObject(parent)
//{
//	helpLab = new QLabel("<small>"+rpar->pd->tooltip +"</small>",p);
//	helpLab->setTextFormat(Qt::RichText);
//	helpLab->setWordWrap(true);
//	helpLab->setVisible(false);
//	helpLab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
//	helpLab->setMinimumWidth(250);
//	helpLab->setMaximumWidth(QWIDGETSIZE_MAX);
//	gridLay = qobject_cast<QGridLayout*>(p->layout());
//	assert(gridLay != 0);
//	row = gridLay->rowCount();
//	gridLay->addWidget(helpLab,row,3,1,1,Qt::AlignTop);
//}


//XMLMeshLabWidget::XMLMeshLabWidget( Expression* expr,QWidget* parent )
//:exp(expr),QObject(parent)
//{
//
//}
