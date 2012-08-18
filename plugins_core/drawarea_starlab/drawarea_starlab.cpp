#include "drawarea_starlab.h"
#include "StarlabMainWindow.h"
#include "StarlabDrawArea.h"
#include "qglviewer.h"

#ifndef GL_MULTISAMPLE
    #define GL_MULTISAMPLE  0x809D
#endif

// Utility Macros
#define Max(a,b) (((a) > (b)) ? (a) : (b))
#define Min(a,b) (((a) < (b)) ? (a) : (b))

using namespace std;

class MyQGLViewer : public StarlabDrawArea, public QGLViewer{
public:
    MyQGLViewer(StarlabApplication* app) : StarlabDrawArea(app){}
        
    virtual void resetViewport(){ setupDefaultCamera(); }
    virtual QGLWidget* widget(){return this;}

    virtual void init(){
        setBackgroundColor(QColor(60,65,70));
        setGridIsDrawn(true);

		// Keyboard + Mouse behavior
		setShortcut(SAVE_SCREENSHOT, Qt::CTRL + Qt::SHIFT + Qt::Key_S);
    }

    void setupDefaultCamera(){
        qglviewer::Vec center(0,0,0);
		
		QBox3D largestBBox;
		largestBBox.setExtents(QVector3D(-1,-1,-1),QVector3D(1,1,1));

        foreach(Model* m, document()->models()){

			const QBox3D bbox = m->getBoundingBox();

			if(bbox.intersects(largestBBox))
				largestBBox = bbox;

            /*Vector3 c = (m->bbox.minimum() + m->bbox.maximum()) * 0.5;
            center = qglviewer::Vec(c[0],c[1],c[2]);
            radius = (m->bbox.maximum() - m->bbox.minimum()).norm() * 0.5;*/
        }

		QVector3D extent(largestBBox.size());
		double radius = Max(extent.x(), Max(extent.y(), extent.z()));

        camera()->setSceneRadius(radius * 2);
        camera()->showEntireScene();
        camera()->setUpVector(qglviewer::Vec(0,0,1));
        camera()->setPosition(qglviewer::Vec(radius,radius,radius) + center);
        camera()->lookAt(center);
    }

    // Draws a spiral
    virtual void draw(){

        // Enable anti-aliasing
        glEnable(GL_MULTISAMPLE);

        /// Render each Model
        foreach(RenderPlugin* renderer, renderers()){
            glPushMatrix();
                glMultMatrixd( document()->transform.data() );
                renderer->render();
            glPopMatrix();        
        }
    }
};

StarlabDrawArea* drawarea_starlab::load(){
    return new MyQGLViewer(application());
}

Q_EXPORT_PLUGIN(drawarea_starlab)
