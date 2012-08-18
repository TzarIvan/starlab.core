#include "GLArea.h"

#include <QDebug>
#include <wrap/gl/space.h>
#include <wrap/gl/picking.h>
#include <wrap/qt/trackball.h>
#include <wrap/qt/col_qt_convert.h>
#include <wrap/qt/shot_qt.h>
// #include <wrap/qt/checkGLError.h>
#include "OpenGLErrorChecker.h"
#include <wrap/qt/gl_label.h>

#include "vcg/math/matrix44.h"
#include "vcg/space/box3.h"
#include "vcg/math/shot.h"

#include "eigen_2_vcg.h"

#include "Document.h"
#include "StarlabSettings.h"

using namespace std;
using namespace vcg;

GLArea::GLArea(StarlabDrawArea *drawArea, Document *document, StarlabSettings *settings) : 
    _drawArea( drawArea ), _document( document ), _settings( settings )
{
       
    animMode=AnimNone;
    iMode=0;
    currentEditor=0;
    suspendedEditor=false;
    lastModelEdited = 0;
    cfps=0;
    lastTime=0;
    hasToPick=false;
    hasToGetPickPos=false;
    hasToUpdateTexture=false;
    helpVisible=false;
    activeDefaultTrackball=true;
    infoAreaVisible = true;
    lastFilterRef = NULL;
    //lastEditRef = NULL;
    setAttribute(Qt::WA_DeleteOnClose,true);
    fov = 60;
    clipRatioFar = 10;
    clipRatioNear = 1;
    nearPlane = .2f;
    farPlane = 5.f;
    setAutoFillBackground(false);
    //Ratser support
    _isRaster =false;
    opacity = 0.5;
    zoom = false;
    targetTex = 0;
}

GLArea::~GLArea() {
#if 0
    // warn any iRender plugin that we're deleting glarea
    if (iRenderer) iRenderer->Finalize(document()->selectedModel, currentShader, this);
    if(targetTex) glDeleteTextures(1, &targetTex);
#endif
}

/*
	This member returns the information of the Mesh in terms of VC,VQ,FC,FQ,WT
	where:
	VC = VertColor,VQ = VertQuality,FC = FaceColor,FQ = FaceQuality,WT = WedgTexCoord
*/
QString GLArea::GetMeshInfoString() {
    QString info;
#ifdef OLDMESHLAB
    if(mm()->hasDataMask(MeshModel::MM_VERTQUALITY) ) {
        info.append("VQ ");
    }
    if(mm()->hasDataMask(MeshModel::MM_VERTCOLOR) )   {
        info.append("VC ");
    }
    if(mm()->hasDataMask(MeshModel::MM_VERTRADIUS) )  {
        info.append("VR ");
    }
    if(mm()->hasDataMask(MeshModel::MM_VERTTEXCOORD)) {
        info.append("VT ");
    }
    if(mm()->hasDataMask(MeshModel::MM_VERTCURV) )    {
        info.append("VK ");
    }
    if(mm()->hasDataMask(MeshModel::MM_VERTCURVDIR) ) {
        info.append("VD ");
    }
    if(mm()->hasDataMask(MeshModel::MM_FACECOLOR) )   {
        info.append("FC ");
    }
    if(mm()->hasDataMask(MeshModel::MM_FACEQUALITY) ) {
        info.append("FQ ");
    }
    if(mm()->hasDataMask(MeshModel::MM_WEDGTEXCOORD)) {
        info.append("WT ");
    }
    if(mm()->hasDataMask(MeshModel::MM_CAMERA) )      {
        info.append("MC ");
    }
    if(mm()->hasDataMask(MeshModel::MM_POLYGONAL) )   {
        info.append("MP ");
    }
#endif
    return info;
}

QSize GLArea::minimumSizeHint() const {
    return QSize(400,300);
}
QSize GLArea::sizeHint() const	{
    return QSize(400,300);
}


void GLArea::initializeGL() {
    // qDebug() << "GLArea::initializeGL()";
    glShadeModel(GL_SMOOTH);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    static float diffuseColor[]= {1.0,1.0,1.0,1.0};
    glEnable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuseColor);
    trackball.center=Point3f(0, 0, 0);
    trackball.radius= 1;

    trackball_light.center=Point3f(0, 0, 0);
    trackball_light.radius= 1;

#ifdef WIN32_
    GLenum err = glewInit();

    if (err != GLEW_OK ) {
        assert(0);
    }
#endif
}



void GLArea::drawGradient() {
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_TRIANGLE_STRIP); 
        glColor4fv(settings()->getColor4fv("glarea/bgtopcolor")); glVertex2f(-1, 1); 
        glColor4fv(settings()->getColor4fv("glarea/bgbotcolor")); glVertex2f(-1,-1); 
        glColor4fv(settings()->getColor4fv("glarea/bgtopcolor")); glVertex2f( 1, 1); 
        glColor4fv(settings()->getColor4fv("glarea/bgbotcolor")); glVertex2f( 1,-1);
    glEnd();

    glPopAttrib();
    glPopMatrix(); // restore modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void GLArea::drawLight() {
// ============== LIGHT TRACKBALL ==============
    // Apply the trackball for the light direction
    glPushMatrix();
    trackball_light.GetView();
    trackball_light.Apply(!(isDefaultTrackBall()));

    static float lightPosF[]= {0.0,0.0,1.0,0.0};
    glLightfv(GL_LIGHT0,GL_POSITION,lightPosF);
    static float lightPosB[]= {0.0,0.0,-1.0,0.0};
    glLightfv(GL_LIGHT1,GL_POSITION,lightPosB);

    if (!(isDefaultTrackBall())) {
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glColor3f(1,1,0);
        glDisable(GL_LIGHTING);
        const unsigned int lineNum=3;
        glBegin(GL_LINES);
        for(unsigned int i=0; i<=lineNum; ++i)
            for(unsigned int j=0; j<=lineNum; ++j) {
                glVertex3f(-1.0f+i*2.0/lineNum,-1.0f+j*2.0/lineNum,-2);
                glVertex3f(-1.0f+i*2.0/lineNum,-1.0f+j*2.0/lineNum, 2);
            }
        glEnd();
        glPopAttrib();
    }
    glPopMatrix();

}

void GLArea::paintEvent(QPaintEvent*){
    QPainter painter(this);
    painter.beginNativePainting();

    makeCurrent();


    if(!isValid() )return;
    QTime time;
    time.start();

    initTexture();
    glClearColor(1.0,1.0,1.0,0.0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setView();  // Set Modelview and Projection matrix
    drawGradient();  // draws the background

    drawLight();


    glPushMatrix();

    // Finally apply the Trackball for the model
    trackball.GetView();
    trackball.Apply(false);
    glPushMatrix();

    //glScale(d);
    //	glTranslate(-FullBBox.Center());
    setLightModel();

    // Set proper colorMode
    if(true /* rm.colorMode != GLSMR::CMNone */) {
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    } 
    else{
        glColor(Color4b::LightGray);
    }
        
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    /// Render each Model
    foreach(RenderPlugin* renderer, drawArea()->renderers()){
        glPushMatrix();
            glMultMatrix( eigen_2_vcg(document()->transform) );
            renderer->render();
        glPopMatrix();        
    }
    
#ifdef TODO_REENABLE_DECORATOR
    /// If the edit plugin needs to do more stuff...
    if(iMode)
        iMode->Decorate(document(),drawArea(),&painter);

    /// Apply decorators
    foreach(Model* m, document()->models()){
        if(!m->isVisible) continue;
        foreach(DecoratePlugin* iDecorator, m->decorators)
            iDecorator->decorate(m, drawArea(), &painter);
    }
#endif
    
    /// Render all the extra "debug" stuff
    drawArea()->drawAllRenderObjects();
    
    glPopAttrib();
    // We restore the state to immediately after the trackball (and before the bbox scaling/translating)
    glPopMatrix(); 

    bool trackBallVisible = settings()->getBool("glarea/trackball");
    if(trackBallVisible && !(iMode && !suspendedEditor))
        trackball.DrawPostApply();

    // The picking of the surface position has to be done in object space,
    // so after trackball transformation (and before the matrix associated to each mesh);
    if(hasToPick && hasToGetPickPos) {
        Point3f pp;
        hasToPick=false;
        if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp)) {
            hasToGetPickPos=false;
        }
    }
    glPopMatrix(); // We restore the state to immediately before the trackball

    // Double click move picked point to center
    // It has to be done in the before trackball space (we MOVE the trackball itself...)
    if(hasToPick && !hasToGetPickPos) {
        Point3f pp;
        hasToPick=false;
        if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp)) {
            trackball.Translate(-pp);
            trackball.Scale(1.25f);
            QCursor::setPos(mapToGlobal(QPoint(width()/2+2,height()/2+2)));
        }
    }

    // Draw the log area background
    // on the bottom of the glArea
    if(infoAreaVisible) {
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_DEPTH_TEST);
        displayInfo(&painter);
        updateFps(time.elapsed());
        glPopAttrib();
    }

    displayViewerHighlight();

    // Finally display HELP if requested
    if (isHelpVisible()) {
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_DEPTH_TEST);
        displayHelp(&painter);
        glPopAttrib();
    }

    OpenGLErrorChecker("glLabel");

    //check if viewers are linked
    painter.endNativePainting();
}

void GLArea::displayMatrix(QPainter* /*painter*/, QRect /*areaRect*/) {
#if 0
    painter->save();
    qFont.setStyleStrategy(QFont::NoAntialias);
    qFont.setFamily("Helvetica");
    qFont.setPixelSize(10);
    painter->setFont(qFont);

    QString tableText;
    for(int i=0; i<4; i++)
        tableText+=QString("\t%1\t%2\t%3\t%4\n")
                   .arg(document()->transform[i][0],5,'f',2).arg(document()->transform[i][1],5,'f',2)
                   .arg(document()->transform[i][2],5,'f',2).arg(document()->transform[i][3],5,'f',2);

    QTextOption TO;
    QTextOption::Tab ttt;
    ttt.type=QTextOption::DelimiterTab;
    ttt.delimiter = '.';
    const int columnSpacing = 40;
    ttt.position=columnSpacing;
    QList<QTextOption::Tab> TabList;
    for(int i=0; i<4; ++i) {
        TabList.push_back(ttt);
        ttt.position+=columnSpacing;
    }
    TO.setTabs(TabList);
    painter->drawText(areaRect, tableText, TO);
    painter->restore();
#endif
}


void GLArea::displayInfo(QPainter * /*painter*/) {
/// @todo re-insert the info area (violet at bottom of traditional meshlab)
#ifdef OLDMESHLAB    
    //glPushAttrib(GL_ALL_ATTRIB_BITS);
    painter->endNativePainting();
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setPen(Qt::white);
    qFont.setStyleStrategy(QFont::NoAntialias);
    qFont.setFamily("Helvetica");
    qFont.setPixelSize(12);
    painter->setFont(qFont);
    float barHeight = qFont.pixelSize()*5;
    QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());

    QRect Column_0(width()/10, this->height()-barHeight+border, width()/2, this->height()-border);
    QRect Column_1(width()/2 , this->height()-barHeight+border, width()*3/4,   this->height()-border);
    QRect Column_2(width()*3/4 , this->height()-barHeight+border, width(),   this->height()-border);

    Color4b logAreaColor = glas.logAreaColor;
    glas.logAreaColor[3]=128;

    painter->fillRect(QRect(0, this->height()-barHeight, width(), this->height()), ColorConverter::ToQColor(logAreaColor));

    QString col1Text,col0Text;


    if(this->md()->size()>0) {
        if(this->md()->size()==1) {
            col1Text += QString("Vertices: %1\n").arg(mm()->n_vertices());
            col1Text += QString("Faces: %1\n").arg(mm()->n_faces());
        } else {
            col1Text += QString("Current Mesh: %1\n").arg(mm()->label());
            col1Text += QString("Vertices: %1 (%2)\n").arg(mm()->n_vertices()).arg(this->md()->vn());
            col1Text += QString("Faces: %1 (%2)\n").arg(mm()->n_faces()).arg(this->md()->fn());
        }

        if(rm.selectedFace || rm.selectedVert || mm()->cm.sfn>0 || mm()->cm.svn>0 )
            col1Text += QString("Selection: v:%1 f:%2\n").arg(mm()->cm.svn).arg(mm()->cm.sfn);

        col1Text += GetMeshInfoString();

        if(fov>5) col0Text += QString("FOV: %1\n").arg(fov);
        else col0Text += QString("FOV: Ortho\n");
        if ((cfps>0) && (cfps<999))
            col0Text += QString("FPS: %1\n").arg(cfps,7,'f',1);
        if ((clipRatioNear!=1) || (clipRatioFar!=1))
            col0Text += QString("Clipping: N:%1 F:%2\n").arg(clipRatioNear,7,'f',1).arg(clipRatioFar,7,'f',1);
        painter->drawText(Column_1, Qt::AlignLeft | Qt::TextWordWrap, col1Text);
        painter->drawText(Column_0, Qt::AlignLeft | Qt::TextWordWrap, col0Text);
        if(mm()->cm.Tr != Matrix44f::Identity() ) displayMatrix(painter, Column_2);
    }
    painter->restore();
    painter->beginNativePainting();
    //glPopAttrib();
#endif
}


void GLArea::displayViewerHighlight() {
    // Enter in 2D screen Mode again
    glPushAttrib(GL_LINE_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1,1,-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f,1.0f,1.0f,0.3f);
    for(int width =5; width>0 ; width -= 2) {
        glLineWidth(width);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-1.f,1.f);
        glVertex2f( 1.f,1.f);
        glVertex2f( 1.f,-1.f);
        glVertex2f(-1.f,-1.f);
        glEnd();
    }
    // Closing 2D
    glPopAttrib();
    glPopMatrix(); // restore modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();

}


void GLArea::displayHelp(QPainter *painter) {
    painter->endNativePainting();
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setPen(Qt::white);
    qFont.setFamily("Helvetica");
    qFont.setPixelSize(14);
    painter->setFont(qFont);

    float bar0Width = qFont.pixelSize()* QString("Ctrl-Shift-Wheel:").size();
    float bar1Width = qFont.pixelSize()* QString("Enter/Exit fullscreen mode").size();
    int border = 12;
       
    QColor logAreaColor = settings()->getQColor("glarea/logareacolor");
    logAreaColor.setBlueF(.5);

    static QString tableText;
    if(tableText.isEmpty()) {
        QFile helpFile(":/images/onscreenHelp.txt");
        if(helpFile.open(QFile::ReadOnly))
            tableText=helpFile.readAll();
        else assert(0);
    }

    QTextOption TO;
    TO.setTabStop(150);

    painter->fillRect(QRect(0,     0,      (bar0Width + bar1Width)+border*2, height()), logAreaColor);
    painter->drawText(QRect(border,border, (bar0Width + bar1Width),          height()-border), tableText,TO);
    painter->restore();
    painter->beginNativePainting();
}

void GLArea::updateDecoration() {
#ifdef OLDMESHLAB
    foreach(DecoratePlugin* p, iDecoratorsList) {
        DecoratePlugin* iDecorator = qobject_cast<DecoratePlugin*>(p);
        iDecorator->endDecorate(model, this);
        iDecorator->startDecorate(model, this);
    }
#endif
}

void GLArea::setCurrentEditAction(QAction *editAction) {
    assert(editAction);
    currentEditor = editAction;
    iMode = actionToMeshEditMap.value(currentEditor);
    assert(iMode);
    lastModelEdited = this->document()->selectedModel();
    iMode->createEdit();
    qDebug("Started plugin: %s", qPrintable(currentEditor->text()));
}


bool GLArea::readyToClose() {
/// CLOSE WITH APP
#if 0
    if (document()->isModified) {
        QMessageBox::StandardButton ret=QMessageBox::question(
                                            this,  tr("StarLab"), tr("Project '%1' modified.\n\nClose without saving?").arg(document()->name),
                                            QMessageBox::Yes|QMessageBox::No,
                                            QMessageBox::No);
        if(ret==QMessageBox::No)	// don't close please!
            return false;
    }
    if(getCurrentEditAction())
        endEdit();
    emit glareaClosed();
#endif
    return true;
}

void GLArea::keyReleaseEvent ( QKeyEvent * e ) {
    if(!isRaster() || isRaster()) {
        e->ignore();
        if(iMode && !suspendedEditor)  iMode->keyReleaseEvent(e);
        else {
            if(e->key()==Qt::Key_Control) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
            if(e->key()==Qt::Key_Shift) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
            if(e->key()==Qt::Key_Alt) trackball.ButtonUp(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
        }
    }
}

void GLArea::keyPressEvent ( QKeyEvent * e ) {
    if(!isRaster() || isRaster()) {
        e->ignore();
        if(iMode && !suspendedEditor)  iMode->keyPressEvent(e);
        else {
            if(e->key()==Qt::Key_Control) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
            if(e->key()==Qt::Key_Shift) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
            if(e->key()==Qt::Key_Alt) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
        }
    }
}

void GLArea::mousePressEvent(QMouseEvent*e) {
    e->accept();
    setFocus();

    if(!isRaster() || isRaster()) {
        if( (iMode && !suspendedEditor) )
            iMode->mousePressEvent(e);
        else {
            if ((e->modifiers() & Qt::ShiftModifier) && (e->modifiers() & Qt::ControlModifier) &&
                    (e->button()==Qt::LeftButton) )
                activeDefaultTrackball=false;
            else activeDefaultTrackball=true;

            if (isDefaultTrackBall()) {
                if(QApplication::keyboardModifiers () & Qt::Key_Control) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
                else trackball.ButtonUp  (QT2VCG(Qt::NoButton, Qt::ControlModifier ) );
                if(QApplication::keyboardModifiers () & Qt::Key_Shift) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
                else trackball.ButtonUp  (QT2VCG(Qt::NoButton, Qt::ShiftModifier ) );
                if(QApplication::keyboardModifiers () & Qt::Key_Alt) trackball.ButtonDown(QT2VCG(Qt::NoButton, Qt::AltModifier ) );
                else trackball.ButtonUp  (QT2VCG(Qt::NoButton, Qt::AltModifier ) );

                trackball.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
            } else trackball_light.MouseDown(e->x(),height()-e->y(), QT2VCG(e->button(), Qt::NoModifier ) );
        }
    }
    update();
}

void GLArea::mouseMoveEvent(QMouseEvent*e) {
    if(!isRaster() || isRaster()) {
        if( (iMode && !suspendedEditor) )
            iMode->mouseMoveEvent(e);
        else {
            if (isDefaultTrackBall()) {
                trackball.MouseMove(e->x(),height()-e->y());
                setCursorTrack(trackball.current_mode);
            } else trackball_light.MouseMove(e->x(),height()-e->y());
        }
        update();
    }

}

// When mouse is released we set the correct mouse cursor
void GLArea::mouseReleaseEvent(QMouseEvent*e) {
    //clearFocus();
    activeDefaultTrackball=true;
    if( (iMode && !suspendedEditor) )
        iMode->mouseReleaseEvent(e);
    else {
        if (isDefaultTrackBall()) trackball.MouseUp(e->x(),height()-e->y(), QT2VCG(e->button(), e->modifiers() ) );
        else trackball_light.MouseUp(e->x(),height()-e->y(), QT2VCG(e->button(),e->modifiers()) );
        setCursorTrack(trackball.current_mode);
    }

    update();
}

//Processing of tablet events, interesting only for painting plugins
void GLArea::tabletEvent(QTabletEvent*e) {
    if(iMode && !suspendedEditor) iMode->tabletEvent(e);
    else e->ignore();
}

void GLArea::wheelEvent(QWheelEvent*e) {
    setFocus();
    const int WHEEL_STEP = 120;
    float notch = e->delta()/ float(WHEEL_STEP);
    switch(e->modifiers()) {
    case Qt::ShiftModifier + Qt::ControlModifier	:
        clipRatioFar  = math::Clamp( clipRatioFar*powf(1.2f, notch),0.01f,50.0f);
        break;
    case Qt::ControlModifier											:
        clipRatioNear = math::Clamp(clipRatioNear*powf(1.2f, notch),0.01f,50.0f);
        break;
    case Qt::AltModifier: 
    {
        qDebug() << "WARNING: [Wheel + Alt] for points size control disabled";
        float pointSize = settings()->getFloat("glarea/pointsize");
        pointSize = math::Clamp(pointSize*powf(1.2f, notch),0.01f,150.0f);
        // foreach(MeshModel * mp, this->md()->meshList)
        //    mp->renderer.pointSize = glas.pointSize;
        break;
    }
    case Qt::ShiftModifier												:
        fov = math::Clamp(fov+1.2f*notch,5.0f,90.0f);
        break;
    default:
        if(isRaster())
            this->opacity = math::Clamp( opacity*powf(1.2f, notch),0.1f,1.0f);
        else
            trackball.MouseWheel( e->delta()/ float(WHEEL_STEP));
        break;
    }
    update();
}


void GLArea::mouseDoubleClickEvent ( QMouseEvent * e ) {
    if(!isRaster() || isRaster()) {
        hasToPick=true;
        pointToPick=Point2i(e->x(),height()-e->y());
    }
    update();
}

void GLArea::setCursorTrack(vcg::TrackMode *tm) {
//static QMap<QString,QCursor> curMap;
    if(curMap.isEmpty()) {
        curMap[QString("")]=QCursor(Qt::ArrowCursor);
        curMap["SphereMode"]=QCursor(QPixmap(":/images/cursors/plain_trackball.png"),1,1);
        curMap["PanMode"]=QCursor(QPixmap(":/images/cursors/plain_pan.png"),1,1);
        curMap["ScaleMode"]=QCursor(QPixmap(":/images/cursors/plain_zoom.png"),1,1);
        curMap["PickMode"]=QCursor(QPixmap(":/images/cursors/plain_pick.png"),1,1);
    }
    if(tm) setCursor(curMap[tm->Name()]);
    else if(hasToGetPickPos) setCursor(curMap["PickMode"]);
    else setCursor(curMap[""]);

}

void GLArea::setDrawMode() {
    // rm.drawMode = mode;
    update();
}


void GLArea::setColorMode() {
    // rm.colorMode = mode;
    update();
}
void GLArea::updateTexture() {
    hasToUpdateTexture = true;
}

/** initTexture loads all the required textures (if necessary).
It is called every time in the glpaint.
It assumes that:
- there is a shared gl wrapper for all the contexts (same texture id for different glareas)
- the values stored in the glwrapper for the texture id are an indicator if there is the need of loading a texture (0 means load that texture)

*/
void GLArea::initTexture() {
#ifdef OLDMESHLAB
    if(hasToUpdateTexture) {
        foreach (MeshModel *mp,this->md()->meshList)
        mp->glw.TMId.clear();

        qDebug("Beware: deleting the texutres could lead to problems for shared textures.");
        hasToUpdateTexture = false;
    }

    foreach (MeshModel *mp, this->md()->meshList) {
        if(!mp->cm.textures.empty() && mp->glw.TMId.empty()) {
            glEnable(GL_TEXTURE_2D);
            GLint MaxTextureSize;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE,&MaxTextureSize);

            for(unsigned int i =0; i< mp->cm.textures.size(); ++i) {
                QImage img, imgScaled, imgGL;
                bool res = img.load(mp->cm.textures[i].c_str());
                if(!res) {
                    // Note that sometimes (in collada) the texture names could have been encoded with a url-like style (e.g. replacing spaces with '%20') so making some other attempt could be harmless
                    QString ConvertedName = QString(mp->cm.textures[i].c_str()).replace(QString("%20"), QString(" "));
                    res = img.load(ConvertedName);
                    if(!res) qDebug("Failure of loading texture %s",mp->cm.textures[i].c_str());
                    else qDebug("Warning, texture loading was successful only after replacing %%20 with spaces;\n Loaded texture %s instead of %s",qPrintable(ConvertedName),mp->cm.textures[i].c_str());
                }
                if(!res && QString(mp->cm.textures[i].c_str()).endsWith("dds",Qt::CaseInsensitive)) {
                    qDebug("DDS binding!");
                    int newTexId = bindTexture(QString(mp->cm.textures[i].c_str()));
                    mp->glw.TMId.push_back(newTexId);
                } else {
                    // image has to be scaled to a 2^n size. We choose the first 2^N <= picture size.
                    int bestW=pow(2.0,floor(::log(double(img.width() ))/::log(2.0)));
                    int bestH=pow(2.0,floor(::log(double(img.height()))/::log(2.0)));
                    while(bestW>MaxTextureSize) bestW /=2;
                    while(bestH>MaxTextureSize) bestH /=2;

                    log->Log(GLLogStream::SYSTEM,"Loading textures");
                    log->Logf(GLLogStream::SYSTEM,"	Texture[ %3i ] =  '%s' ( %6i x %6i ) -> ( %6i x %6i )",	i,mp->cm.textures[i].c_str(), img.width(), img.height(),bestW,bestH);
                    imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                    imgGL=convertToGLFormat(imgScaled);
                    mp->glw.TMId.push_back(0);
                    qDebug("      	will be loaded as GL texture id %i  ( %i x %i )",mp->glw.TMId.back() ,imgGL.width(), imgGL.height());
                    glGenTextures( 1, (GLuint*)&(mp->glw.TMId.back()) );
                    glBindTexture( GL_TEXTURE_2D, mp->glw.TMId.back() );
                    glTexImage2D( GL_TEXTURE_2D, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
                    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgGL.width(), imgGL.height(), GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
                }
                if(glas.textureMagFilter == 0 ) 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
                else	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                if(glas.textureMinFilter == 0 ) 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
                else	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            }
        }
        glDisable(GL_TEXTURE_2D);
    }
#endif
}

void GLArea::setLightModel() {
    /// @todo fix lighting
    if(true){
        glEnable(GL_LIGHTING);

        /// @todo fix here
        if (false)
            glEnable(GL_LIGHT1);
        else
            glDisable(GL_LIGHT1);

        glLightfv(GL_LIGHT0, GL_AMBIENT,  settings()->getColor4fv("glarea/baselight_ambientcolor"));
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  settings()->getColor4fv("glarea/baselight_diffusecolor"));
        glLightfv(GL_LIGHT0, GL_SPECULAR, settings()->getColor4fv("glarea/baselight_specularcolor"));
        glLightfv(GL_LIGHT1, GL_AMBIENT,  settings()->getColor4fv("glarea/baselight_ambientcolor"));
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  settings()->getColor4fv("glarea/baselight_diffusecolor"));
        glLightfv(GL_LIGHT1, GL_SPECULAR, settings()->getColor4fv("glarea/baselight_specularcolor"));
        
#if OLDMESHLAB
        /// @todo fix fancy lighting
        if( false /*rm.fancyLighting*/) {
            // glLightfv(GL_LIGHT0, GL_DIFFUSE, Color4f::Construct(glas.fancyFLightDiffuseColor).V());
            // glLightfv(GL_LIGHT1, GL_DIFFUSE, Color4f::Construct(glas.fancyBLightDiffuseColor).V());
        }
#endif
    } 
        else glDisable(GL_LIGHTING);
}

void GLArea::setView() {
    glViewport(0,0, this->width(),this->height());
    curSiz.setWidth(this->width());
    curSiz.setHeight(this->height());

    GLfloat fAspect = (GLfloat)curSiz.width()/ curSiz.height();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // This parameter is the one that controls:
    // HOW LARGE IS THE TRACKBALL ICON ON THE SCREEN.
    float viewRatio = 1.75f;
    float cameraDist = viewRatio / tanf(math::ToRad(fov*.5f));

    if(fov==5)
        cameraDist = 1000; // small hack for orthographic projection where camera distance is rather meaningless...
    nearPlane = cameraDist - 2.f*clipRatioNear;
    farPlane =  cameraDist + 10.f*clipRatioFar;
    if(nearPlane<=cameraDist*.1f) nearPlane=cameraDist*.1f;

    if(fov==5)	
        glOrtho( -viewRatio*fAspect, viewRatio*fAspect, -viewRatio, viewRatio, cameraDist - 2.f*clipRatioNear, cameraDist+2.f*clipRatioFar);
    else    		
        gluPerspective(fov, fAspect, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, cameraDist,0, 0, 0, 0, 1, 0);
}

void GLArea::updateFps(float deltaTime) {
    static float fpsVector[10];
    static int j=0;
    float averageFps=0;
    if (deltaTime>0) {
        fpsVector[j]=deltaTime;
        j=(j+1) % 10;
    }
    for (int i=0; i<10; i++) averageFps+=fpsVector[i];
    cfps=1000.0f/(averageFps/10);
    lastTime=deltaTime;
}

void GLArea::normalizeTrackball() {
    const vcg::Box3f docbbox = eigen_2_vcg(document()->bbox());
    trackball.Reset();
    float newScale= 3.0f/docbbox.Diag();
    trackball.track.sca = newScale;
    trackball.track.tra = -docbbox.Center();
    update();
}

void GLArea::hideEvent(QHideEvent * /*event*/) {
    trackball.current_button=0;
}



vcg::Point3f GLArea::getViewDir() {
    vcg::Matrix44f rotM;
    trackball.track.rot.ToMatrix(rotM);
    vcg::Invert(rotM);
    return rotM*vcg::Point3f(0,0,1);
}

void GLArea::setTarget(QImage &image) {

    if (targetTex) {
        glDeleteTextures(1, &targetTex);
        targetTex = 0;
    }
    // create texture
    glGenTextures(1, &targetTex);
    QImage tximg = QGLWidget::convertToGLFormat(image);
    glBindTexture(GL_TEXTURE_2D, targetTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tximg.width(), tximg.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, tximg.bits());

    glBindTexture(GL_TEXTURE_2D, 0);
}

// --------------- Methods involving shots -------------------------------------

float GLArea::getCameraDistance() {
    // This parameter is the one that controls:
    // HOW LARGE IS THE TRACKBALL ICON ON THE SCREEN.
    float viewRatio = 1.75f;
    float cameraDist = viewRatio / tanf(vcg::math::ToRad(fov*.5f));

    return cameraDist;
}

void GLArea::initializeShot(Shotf &shot) {
    shot.Intrinsics.PixelSizeMm[0]=0.036916077;
    shot.Intrinsics.PixelSizeMm[1]=0.036916077;

    shot.Intrinsics.DistorCenterPx[0]=width()/2;
    shot.Intrinsics.DistorCenterPx[1]=height()/2;
    shot.Intrinsics.CenterPx[0]=width()/2;
    shot.Intrinsics.CenterPx[1]=height()/2;
    shot.Intrinsics.ViewportPx[0]=width();
    shot.Intrinsics.ViewportPx[1]=height();

    double viewportYMm = shot.Intrinsics.PixelSizeMm[1]*shot.Intrinsics.ViewportPx[1];
    float defaultFov=60.0;
    shot.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(defaultFov/2))); //27.846098mm

    shot.Extrinsics.SetIdentity();
}

bool GLArea::viewFromFile() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Load Project"), "./", tr("Xml Files (*.xml)"));

    QFile qf(filename);
    QFileInfo qfInfo(filename);

    if( !qf.open(QIODevice::ReadOnly ) )
        return false;

    QString project_path = qfInfo.absoluteFilePath();

    QDomDocument doc("XmlDocument");    //It represents the XML document
    if(!doc.setContent( &qf ))
        return false;

    QString type = doc.doctype().name();

    //TextAlign file project
    if(type == "RegProjectML")   loadShotFromTextAlignFile(doc);
    //View State file
    else if(type == "ViewState") loadViewFromViewStateFile(doc);

    qDebug("End file reading");
    qf.close();

    return true;
}

void GLArea::loadShot(const QPair<vcg::Shotf,float> &shotAndScale){
	
  Shotf shot = shotAndScale.first;
	
	fov = shot.GetFovFromFocal();

	float cameraDist = getCameraDistance();

	//reset trackball. The point of view must be set only by the shot
	trackball.Reset();
	trackball.track.sca = shotAndScale.second;
	
  /*Point3f point = this->md()->bbox().Center();
	Point3f p1 = ((trackball.track.Matrix()*(point-trackball.center))- Point3f(0,0,cameraDist));*/
	shot2Track(shot, cameraDist,trackball);

	//Expressing the translation along Z with a scale factor k
	//Point3f p2 = ((trackball.track.Matrix()*(point-trackball.center))- Point3f(0,0,cameraDist));

	////k is the ratio between the distances along z of two correspondent points (before and after the traslation) 
	////from the point of view
	//float k= abs(p2.Z()/p1.Z());

	//float sca= trackball.track.sca/k;
	//Point3f tra = trackball.track.tra;
	//
	//// Apply this formula:
	//// SR(t+p) -v = k[S'R'(t'+p) -v] forall p, R=R', k is a costant
	//// SR(t) -v = k[S'R(t') -v]
	//// t' = 1/k* S'^-1St + (k-1)/k S'^-1*R^-1v
	//Matrix44f s0 = Matrix44f().SetScale(trackball.track.sca,trackball.track.sca, trackball.track.sca);
	//Matrix44f s1 = Matrix44f().SetScale(sca, sca, sca);
	//Matrix44f r;
	//trackball.track.rot.ToMatrix(r);
	//Matrix44f rapM = Matrix44f().SetScale(1/k, 1/k, 1/k);
	//Matrix44f rapM2 = Matrix44f().SetScale(1-1/k, 1-1/k, 1-1/k);
	//Point3f t1 = rapM*Inverse(s1)*s0*tra + rapM2*Inverse(s1)*Inverse(r)*Point3f(0,0,cameraDist);

	//trackball.track.sca =sca;
	//trackball.track.tra =t1 /*+ tb.track.rot.Inverse().Rotate(glLookAt)*/ ;


	update();
}

void GLArea::loadShotFromTextAlignFile(const QDomDocument &doc) {
    QDomElement root = doc.documentElement();
    QDomNode node;
    Shotf shot;

    node = root.firstChild();

    //Devices
    while(!node.isNull()) {
        if(QString::compare(node.nodeName(),"Device")==0) {
            QString type = node.attributes().namedItem("type").nodeValue();
            if (type== "GlImageWidget") {
                //Aligned Image
                if(QString::compare(node.attributes().namedItem("aligned").nodeValue(),"1")==0) {
                    QDomNode nodeb = node.firstChild();
                    ReadShotFromOLDXML(shot,nodeb);
                }
            }
        }
        node = node.nextSibling();
    }

    //Adjust params for Meshlab settings

    //resize viewport
    int w = shot.Intrinsics.ViewportPx[0];
    int h = shot.Intrinsics.ViewportPx[1];

    shot.Intrinsics.DistorCenterPx[0]=w/2;
    shot.Intrinsics.DistorCenterPx[1]=h/2;
    shot.Intrinsics.CenterPx[0]=w/2;
    shot.Intrinsics.CenterPx[1]=h/2;
    shot.Intrinsics.ViewportPx[0]=w;
    shot.Intrinsics.ViewportPx[1]=h;

    // The shot loaded from TextAlign doesn't have a scale. Trackball needs it.
    // The scale factor is computed as the ratio between cameraDistance and the z coordinate of the translation
    // introduced by the shot.
    fov = shot.GetFovFromFocal();

    float cameraDist = getCameraDistance();

    vcg::Matrix44f rotFrom;
    shot.Extrinsics.Rot().ToMatrix(rotFrom);

    Point3f p1 = rotFrom*(vcg::Point3f::Construct(shot.Extrinsics.Tra()));

    Point3f p2 = (Point3f(0,0,cameraDist));

    trackball.track.sca =fabs(p2.Z()/p1.Z());

    loadShot(QPair<Shotf, float> (shot,trackball.track.sca));

}

/*
ViewState file is an xml file format created by Meshlab with the action "copyToClipboard"
*/
void GLArea::loadViewFromViewStateFile(const QDomDocument &doc) {
    Shotf shot;
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();

    while(!node.isNull()) {
        if (QString::compare(node.nodeName(),"VCGCamera")==0)
            ReadShotFromQDomNode<Shotf>(shot,node);
        else if (QString::compare(node.nodeName(),"CamParam")==0)
            ReadShotFromOLDXML<Shotf>(shot,node);

        else if (QString::compare(node.nodeName(),"ViewSettings")==0) {
            QDomNamedNodeMap attr = node.attributes();
            trackball.track.sca = attr.namedItem("TrackScale").nodeValue().section(' ',0,0).toFloat();
            nearPlane = attr.namedItem("NearPlane").nodeValue().section(' ',0,0).toFloat();
            farPlane = attr.namedItem("FarPlane").nodeValue().section(' ',0,0).toFloat();
            fov = shot.GetFovFromFocal();
            clipRatioNear = (getCameraDistance()-nearPlane)/2.0f ;
            clipRatioFar = (farPlane-getCameraDistance())/10.0f ;

        } else if (QString::compare(node.nodeName(),"Render")==0) {
            QDomNamedNodeMap attr = node.attributes();
#ifdef OLDMESHLAB
            rm.drawMode = (GLSMR::DrawMode) (attr.namedItem("DrawMode").nodeValue().section(' ',0,0).toInt());
            rm.colorMode = (GLSMR::ColorMode) (attr.namedItem("ColorMode").nodeValue().section(' ',0,0).toInt());
            rm.textureMode = (GLSMR::TextureMode) (attr.namedItem("TextureMode").nodeValue().section(' ',0,0).toInt());
            rm.lighting = (attr.namedItem("Lighting").nodeValue().section(' ',0,0).toInt() != 0);
            rm.backFaceCull = (attr.namedItem("BackFaceCull").nodeValue().section(' ',0,0).toInt() != 0);
            rm.doubleSideLighting = (attr.namedItem("DoubleSideLighting").nodeValue().section(' ',0,0).toInt() != 0);
            rm.fancyLighting = (attr.namedItem("FancyLighting").nodeValue().section(' ',0,0).toInt() != 0);
            rm.selectedFace = (attr.namedItem("SelectedFace").nodeValue().section(' ',0,0).toInt() != 0);
            rm.selectedVert = (attr.namedItem("SelectedVert").nodeValue().section(' ',0,0).toInt() != 0);
#endif
        }
        node = node.nextSibling();
    }

    loadShot(QPair<Shotf, float> (shot,trackball.track.sca));
}

void GLArea::viewToClipboard() {
    QClipboard *clipboard = QApplication::clipboard();
    Shotf shot = shotFromTrackball().first;

    QDomDocument doc("ViewState");
    QDomElement root = doc.createElement("project");
    doc.appendChild( root );

    QDomElement shotElem = WriteShotToQDomNode(shot,doc);
    root.appendChild(shotElem);

    QDomElement settingsElem = doc.createElement( "ViewSettings" );
    settingsElem.setAttribute( "TrackScale", trackball.track.sca);
    settingsElem.setAttribute( "NearPlane", nearPlane);
    settingsElem.setAttribute( "FarPlane", farPlane);
    root.appendChild(settingsElem);

    QDomElement renderElem = doc.createElement( "Render");
#ifdef OLDMESHLAB
    renderElem.setAttribute("DrawMode",rm.drawMode);
    renderElem.setAttribute("ColorMode",rm.colorMode);
    renderElem.setAttribute("TextureMode",rm.textureMode);
    renderElem.setAttribute("Lighting",rm.lighting);
    renderElem.setAttribute("BackFaceCull",rm.backFaceCull);
    renderElem.setAttribute("DoubleSideLighting",rm.doubleSideLighting);
    renderElem.setAttribute("FancyLighting",rm.fancyLighting);
    renderElem.setAttribute("SelectedFace",rm.selectedFace);
    renderElem.setAttribute("SelectedVert",rm.selectedVert);
#endif
    root.appendChild(renderElem);

    clipboard->setText(doc.toString()); //.remove(QChar('\n')));
}

void GLArea::viewFromClipboard() {
    QClipboard *clipboard = QApplication::clipboard();
    QString shotString = clipboard->text();
    QDomDocument doc("StringDoc");
    doc.setContent(shotString);
    loadViewFromViewStateFile(doc);
}

QPair<vcg::Shotf,float> GLArea::shotFromTrackball() {
    Shotf shot;
    initializeShot(shot);

    double viewportYMm=shot.Intrinsics.PixelSizeMm[1]*shot.Intrinsics.ViewportPx[1];
    shot.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(fov/2)));

    float cameraDist = getCameraDistance();

    //add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
    //T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
    //Add translation S^(-1) R^(-1)(gl)
    //Shotd doesn't introduce scaling
    //---------------------------------------------------------------------
    shot.Extrinsics.SetTra( shot.Extrinsics.Tra() + (Inverse(shot.Extrinsics.Rot())*Point3f(0, 0, cameraDist)));

    vcg::Shotf newShot = track2ShotCPU<float>(shot, &trackball);

    return QPair<Shotf, float> (newShot,trackball.track.sca);
}


void GLArea::createOrthoView(QString dir) {
    Shotf view;
    initializeShot(view);

    fov =5;
    double viewportYMm = view.Intrinsics.PixelSizeMm[1]*view.Intrinsics.ViewportPx[1];
    view.Intrinsics.FocalMm = viewportYMm/(2*tanf(vcg::math::ToRad(fov/2))); //27.846098 equivalente a circa 60 gradi

    const vcg::Box3f docbbox = eigen_2_vcg(document()->bbox());
    trackball.Reset();
    float newScale= 3.0f/docbbox.Diag();
    trackball.track.sca = newScale;
    trackball.track.tra = -docbbox.Center();

    vcg::Matrix44f rot;

    if(dir == tr("Top"))
        rot.SetRotateDeg(90,Point3f(1,0,0));
    else if(dir == tr("Bottom"))
        rot.SetRotateDeg(90,Point3f(-1,0,0));
    else if(dir == tr("Left"))
        rot.SetRotateDeg(90,Point3f(0,1,0));
    else if(dir == tr("Right"))
        rot.SetRotateDeg(90,Point3f(0,-1,0));
    else if(dir == tr("Front"))
        rot.SetRotateDeg(0,Point3f(0,1,0));
    else if(dir == tr("Back"))
        rot.SetRotateDeg(180,Point3f(0,1,0));

    view.Extrinsics.SetRot(rot);

    float cameraDist = getCameraDistance();

    //add the translation introduced by gluLookAt() (0,0,cameraDist), in order to have te same view---------------
    //T(gl)*S*R*T(t) => SR(gl+t) => S R (S^(-1)R^(-1)gl + t)
    //Add translation S^(-1) R^(-1)(gl)
    //Shotd doesn't introduce scaling
    //---------------------------------------------------------------------
    view.Extrinsics.SetTra( view.Extrinsics.Tra() + (Inverse(view.Extrinsics.Rot())*Point3f(0, 0, cameraDist)));

    Shotf shot = track2ShotCPU(view, &trackball);

    QPair<Shotf,float> shotAndScale = QPair<Shotf,float> (shot, trackball.track.sca);
    loadShot(shotAndScale);
}

void GLArea::viewFrom(QAction* from){
    if(from==NULL) 
        resetView();
    else
        selectSimpleView(from->text());
}
