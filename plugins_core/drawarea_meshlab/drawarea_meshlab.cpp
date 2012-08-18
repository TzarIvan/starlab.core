#include "drawarea_meshlab.h"
#include "GLArea.h"
#include "StarlabMainWindow.h"

/// This is a wrapper for DrawArea 
class MeshlabDrawArea : public StarlabDrawArea{

    /// @{ basic plugni setup    
    private: 
        GLArea* _widget;
    public:
        MeshlabDrawArea(StarlabApplication* app) 
            : StarlabDrawArea( app ){
            _widget = new GLArea(this,app->document(),app->settings());
        }
        
        void resetViewport(){ 
            // qDebug() << "MeshlabDrawArea::resetView()";
            _widget->resetView(); 
        }
        QGLWidget* widget(){ return _widget; }
    /// @}
};

StarlabDrawArea* drawarea_meshlab::load(){
    /// Setup the settings    
    /// @todo move this into constructor of glarea
    settings()->setDefault("glarea/trackball",false);
    settings()->setDefault("glarea/bgbotcolor",QColor(128,128,255));
    settings()->setDefault("glarea/bgtopcolor",QColor(0,0,0));
    settings()->setDefault("glarea/baselight_ambientcolor",QColor(32,32,32));
    settings()->setDefault("glarea/baselight_diffusecolor",QColor(204,204,204));
    settings()->setDefault("glarea/baselight_specularcolor",QColor(255,255,255));
    settings()->setDefault("glarea/pointsize",1.0);    
    settings()->setDefault("glarea/twosidedface",false);  
    
    MeshlabDrawArea* drawArea = new MeshlabDrawArea(application());
    
    /// Create actions & add them to menu
    /// Parent menu entries with drawarea so they are removed on a drawarea switch
    {
        QAction* twosided  = new QAction("Two sided lighting",drawArea);
        mainWindow()->viewMenu->addAction(twosided);
        twosided->setCheckable(true);
        QAction* separator = mainWindow()->viewMenu->insertSeparator(twosided);
        separator->setParent(drawArea);
        twosided->setChecked(settings()->getBool("glarea/twosidedface"));
        connect(twosided,SIGNAL(toggled(bool)),this,SLOT(toggleTwoSidedLighting(bool)));
    }
    {
        QAction* action = new QAction("Fancy background",drawArea);
        mainWindow()->viewMenu->addAction(action);
        action->setCheckable(true);
        action->setChecked(true);
        connect(action,SIGNAL(toggled(bool)),this,SLOT(toggleBackground(bool)));
    }
    {
        QAction* showtrackball = new QAction("Show trackball",drawArea);
        mainWindow()->viewMenu->addAction(showtrackball);
        showtrackball->setCheckable(true);
        /// @todo connect to the slot
    }
      
    return drawArea;
}

void drawarea_meshlab::toggleTwoSidedLighting(bool twosided){
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, twosided?GL_TRUE:GL_FALSE);
    mainWindow()->updateDrawArea();
}

void drawarea_meshlab::toggleBackground(bool useMeshlab){
    if(useMeshlab){
        settings()->set("glarea/bgbotcolor",QColor(128,128,255));
        settings()->set("glarea/bgtopcolor",QColor(0,0,0));
    } else {
        settings()->set("glarea/bgbotcolor",QColor(255,255,255));
        settings()->set("glarea/bgtopcolor",QColor(255,255,255));        
    }    
    mainWindow()->updateDrawArea();
}

Q_EXPORT_PLUGIN(drawarea_meshlab)
