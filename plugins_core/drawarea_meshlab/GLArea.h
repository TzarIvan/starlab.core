#pragma once

#include <QDomDocument>
#include <GL/glew.h>
#include <QGLWidget>
#include <QTimer>
#include <QTime>
#include <QtGui>
#include <vcg/space/plane3.h>
#include <vcg/space/line3.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/math.h>
#include <wrap/gui/trackball.h>
#include <vcg/math/shot.h>
#include <wrap/gl/shot.h>
#include "Document.h"
#include "StarlabSettings.h"
#include "StarlabDrawArea.h"

#include "interfaces/DecoratePlugin.h"
#include "interfaces/RenderPlugin.h"
#include "interfaces/ModePlugin.h"


#define SSHOT_BYTES_PER_PIXEL 4


class MeshModel;
class StarlabMainWindow;
class Document;

class GLArea : public QGLWidget{
    /// @{ 
    private:
        StarlabDrawArea* const _drawArea; 
        Document* const        _document;
        StarlabSettings* const _settings;
    protected:
        Document* document(){ return _document; }
        StarlabDrawArea* drawArea(){ return _drawArea; }
        StarlabSettings* settings(){ return _settings; }
    /// @}
    
public:
    // Make sure we use VCG's in this class
    typedef vcg::Point3f Point3f;
    
public slots:
    virtual void viewFrom(QAction* from = NULL);
    virtual void resetView(){ normalizeTrackball(); }  
    virtual void selectSimpleView(QString viewname){ createOrthoView(viewname); }
    virtual void viewToClipboard();
    virtual void viewFromClipboard();
    
public:
    GLArea(StarlabDrawArea* drawArea, Document* document, StarlabSettings* settings);
    ~GLArea();
    

    
    vcg::Trackball trackball;
    vcg::Trackball trackball_light;
    QSize curSiz;
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    QAction *getLastAppliedFilter()							{
        return lastFilterRef;
    }
    void		setLastAppliedFilter(QAction *qa)		{
        lastFilterRef = qa;
    }

    void updateFps(float deltaTime);

    bool isHelpVisible(){
        return helpVisible;
    }
    bool isDefaultTrackBall(){
        return activeDefaultTrackball;
    }
    
    void toggleHelpVisible()      {
        helpVisible = !helpVisible;
        update();
    }
    void saveSnapshot();
    void setLightModel();
    void setView();
    void normalizeTrackball();

    
    // Edit Mode management
    // In the glArea we can have a active Editor that can toggled into a ''suspendeed'' state
    // in which the mouse event are redirected back to the GLArea to drive the camera trackball
    // The decorate function of the current active editor is still called.
    // endEdit is called only when you press again the same button or when you change editor.

public slots:
    void updateTexture(); // slot for forcing the texture reload.

    //slots for changing the draw rendering and texturing mode
    void setDrawMode();
    void setColorMode();

    void endEdit() {
#if 0
        if(iMode && currentEditor) {
            iMode->destroyEdit(document(),this);
        }
        iMode= 0;
        currentEditor=0;
        setCursorTrack(0);
        update();
        emit updateMainWindowMenus();
#endif
    }

    /// @deprecated
    void suspendEditToggle() {
        if(currentEditor==0) return;
        static QCursor qc;
        if(suspendedEditor) {
            suspendedEditor=false;
            setCursor(qc);
        }	else {
            suspendedEditor=true;
            qc=cursor();
            setCursorTrack(0);
        }
    }

signals:
    void updateMainWindowMenus(); //updates the menus of the meshlab MainWindow
    void glareaClosed();					//someone has closed the glarea

    void currentViewerChanged(int currentId);

public slots:
    // Called when we change layer, notifies the edit tool if one is open
    void updateDecoration();

public:
    //call when the editor changes
    void setCurrentEditAction(QAction *editAction);

    //get the currently active edit action
    QAction * getCurrentEditAction() {
        return currentEditor;
    }

    //get the currently active mesh editor
    ModePlugin * getCurrentMeshEditor() {
        return iMode;
    }

    //see if this glAarea has a MESHEditInterface for this action
    bool editorExistsForAction(QAction *editAction) {
        return actionToMeshEditMap.contains(editAction);
    }

    //add a MeshEditInterface for the given action
    void addMeshEditor(QAction *editAction, ModePlugin* editor) {
        actionToMeshEditMap.insert(editAction, editor);
    }
    bool readyToClose();
    float lastRenderingTime() {
        return lastTime;
    }
    void drawGradient();
    void drawLight();
    float getFov() {
        return fov;
    }
    
    void updateLayerTable();

public:
    vcg::Point3f getViewDir();
    bool	infoAreaVisible;		// Draws the lower info area ?
    bool  suspendedEditor;
protected:

    void initializeGL();
    void initTexture();
    void displayInfo(QPainter *painter);
    void displayMatrix(QPainter *painter, QRect areaRect);
    void displayViewerHighlight();
    void displayHelp(QPainter *painter);

    QString GetMeshInfoString();
    void paintEvent(QPaintEvent *event);
    void keyReleaseEvent ( QKeyEvent * e );
    void keyPressEvent ( QKeyEvent * e );
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent ( QMouseEvent * event ) ;
    void wheelEvent(QWheelEvent*e);
    void tabletEvent(QTabletEvent *e);
    bool drawSelection;
    void hideEvent(QHideEvent * event);

private:
    QMap<QString,QCursor> curMap;
    void pasteTile();
    void setTiledView(GLdouble fovY, float viewRatio, float fAspect, GLdouble zNear, GLdouble zFar, float cameraDist);

    bool  helpVisible;				// Help on screen
    bool  activeDefaultTrackball; // keep track on active trackball
    bool  hasToUpdateTexture; // has to reload textures at the next redraw
    bool  hasToPick;							// has to pick during the next redraw.
    bool hasToGetPickPos;							// if we are waiting for a double click for getting a surface position that has to be sent back using signal/slots (for parameters)
    QString nameToGetPickPos;         // the name of the parameter that has asked for the point on the surface

    vcg::Point2i pointToPick;

    //shader support
    /// @deprecated
    QAction* currentEditor;
    QAction *lastFilterRef; // reference to last filter applied
    QFont	qFont;			//font settings

    // Editing support
    ModePlugin *iMode;
    /// @deprecated reference to last Editing Mode Used
    QAction *suspendedEditRef; 
    /// @todo rename this variable appropriately?
    QMap<QAction*, ModePlugin*> actionToMeshEditMap;

    //the last model that start edit was called with
    Model *lastModelEdited;

public:
    // view setting variables
    float fov;
    float clipRatioFar;
    float clipRatioNear;
    float nearPlane;
    float farPlane;

private:
    float cfps;
    float lastTime;

    QImage snapBuffer;

    enum AnimMode { AnimNone, AnimSpin, AnimInterp};
    AnimMode animMode;
    int tileCol, tileRow, totalCols, totalRows;   // snapshot: total number of subparts and current subpart rendered
    int  currSnapLayer;            // snapshot: total number of layers and current layer rendered
    void setCursorTrack(vcg::TrackMode *tm);

    //-----------Raster support----------------------------
private:
    bool _isRaster; // true if the viewer is a RasterViewer, false if is a MeshViewer; default value is false.

    int zoomx, zoomy;
    bool zoom;
    float opacity;
    GLuint targetTex;           // here we store the reference image. The raster image is rendered as a texture

public:
    bool isRaster() {
        return _isRaster;
    }
    void setIsRaster(bool viewMode);
    void loadRaster(int id);

    void setTarget(QImage &image);

private:

    //-----------Shot support----------------------------
public:
    QPair<vcg::Shotf, float > shotFromTrackball();
    bool viewFromFile();
    void createOrthoView(QString);
    void loadShot(const QPair<vcg::Shotf, float> &) ;

private:

    float getCameraDistance();
    void initializeShot(vcg::Shotf &shot);
    void loadShotFromTextAlignFile(const QDomDocument &doc);
    void loadViewFromViewStateFile(const QDomDocument &doc);


    /*
    Given a shot "refCamera" and a trackball "track", computes a new shot which is equivalent
    to apply "refCamera" o "track" (via GPU).
    */
    template <class T>
    vcg::Shot<T> track2ShotGPU(vcg::Shot<T> &refCamera, vcg::Trackball *track) {
        vcg::Shot<T> view;

        double _near, _far;
        _near = 0.1;
        _far = 100;

        //get OpenGL modelview matrix after applying the trackball
        GlShot<vcg::Shot<T> >::SetView(refCamera, _near, _far);
        glPushMatrix();
        track->GetView();
        track->Apply();
        vcg::Matrix44d model;
        glGetv(GL_MODELVIEW_MATRIX, model);
        glPopMatrix();
        GlShot<vcg::Shot<T> >::UnsetView();

        //get translation out of modelview
        vcg::Point3d tra;
        tra[0] = model[0][3];
        tra[1] = model[1][3];
        tra[2] = model[2][3];
        model[0][3] = model[1][3] = model[2][3] = 0;

        //get pure rotation out of modelview
        double det = model.Determinant();
        double idet = 1/pow(det, 1/3.0); //inverse of the determinant
        model *= idet;
        model[3][3] = 1;
        view.Extrinsics.SetRot(model);

        //get pure translation out of modelview
        vcg::Matrix44d imodel = model;
        vcg::Transpose(imodel);
        tra = -(imodel*tra);
        tra *= idet;
        view.Extrinsics.SetTra(vcg::Point3<T>::Construct(tra));

        //use same current intrinsics
        view.Intrinsics = refCamera.Intrinsics;

        return view;
    }

    /*
    Given a shot "refCamera" and a trackball "track", computes a new shot which is equivalent
    to apply "refCamera" o "track" (via CPU).
    */
    template <class T>
    vcg::Shot<T> track2ShotCPU(vcg::Shot<T> &refCamera, vcg::Trackball *track) {
        vcg::Shot<T> view;

        double _near, _far;
        _near = 0.1;
        _far = 100;

        //get shot extrinsics matrix
        vcg::Matrix44<T> shotExtr;
        refCamera.GetWorldToExtrinsicsMatrix().ToMatrix(shotExtr);

        vcg::Matrix44<T> model2;
        model2 = (shotExtr)* track->Matrix();
        vcg::Matrix44<T> model;
        model2.ToMatrix(model);

        //get translation out of modelview
        vcg::Point3<T> tra;
        tra[0] = model[0][3];
        tra[1] = model[1][3];
        tra[2] = model[2][3];
        model[0][3] = model[1][3] = model[2][3] = 0;

        //get pure rotation out of modelview
        double det = model.Determinant();
        double idet = 1/pow(det, 1/3.0); //inverse of the determinant
        model *= idet;
        model[3][3] = 1;
        view.Extrinsics.SetRot(model);

        //get pure translation out of modelview
        vcg::Matrix44<T> imodel = model;
        vcg::Transpose(imodel);
        tra = -(imodel*tra);
        tra *= idet;
        view.Extrinsics.SetTra(vcg::Point3<T>::Construct(tra));

        //use same current intrinsics
        view.Intrinsics = refCamera.Intrinsics;

        return view;
    }

    /*
    Given a shot "from" and a trackball "track", updates "track" with "from" extrinsics.
    A traslation involving cameraDistance is included. This is necessary to compensate a trasformation that OpenGL performs
    at the end of the graphic pipeline.
    */
    template <class T>
    void shot2Track(const vcg::Shot<T> &from, const float cameraDist, vcg::Trackball &tb) {

        vcg::Quaternion<T> qfrom;
        qfrom.FromMatrix(from.Extrinsics.Rot());

        tb.track.rot = vcg::Quaternionf::Construct(qfrom);
        tb.track.tra =  (vcg::Point3f::Construct(-from.Extrinsics.Tra()));
        tb.track.tra += vcg::Point3f::Construct(tb.track.rot.Inverse().Rotate(vcg::Point3f(0,0,cameraDist)))*(1/tb.track.sca);
    }

};
