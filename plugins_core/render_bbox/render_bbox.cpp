#include "render_bbox.h"
#include <qgl.h>

void render_bbox::render(){
    /// Setup BBOX color
    QBox3D bbox = model()->bbox();
    QColor& c = model()->color;
    glColor3f(c.redF(),c.greenF(),c.blueF());
    
    float min[3]; 
    min[0] = bbox.minimum().x();
    min[1] = bbox.minimum().y();
    min[2] = bbox.minimum().z();

    float max[3]; 
    max[0] = bbox.maximum().x();
    max[1] = bbox.maximum().y();
    max[2] = bbox.maximum().z();
        
    /// --- Inherited from VCG ---
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_STRIP);
    glVertex3f((float)min[0],(float)min[1],(float)min[2]);
    glVertex3f((float)max[0],(float)min[1],(float)min[2]);
    glVertex3f((float)max[0],(float)max[1],(float)min[2]);
    glVertex3f((float)min[0],(float)max[1],(float)min[2]);
    glVertex3f((float)min[0],(float)min[1],(float)min[2]);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f((float)min[0],(float)min[1],(float)max[2]);
    glVertex3f((float)max[0],(float)min[1],(float)max[2]);
    glVertex3f((float)max[0],(float)max[1],(float)max[2]);
    glVertex3f((float)min[0],(float)max[1],(float)max[2]);
    glVertex3f((float)min[0],(float)min[1],(float)max[2]);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f((float)min[0],(float)min[1],(float)min[2]);
    glVertex3f((float)min[0],(float)min[1],(float)max[2]);
    glVertex3f((float)max[0],(float)min[1],(float)min[2]);
    glVertex3f((float)max[0],(float)min[1],(float)max[2]);
    glVertex3f((float)max[0],(float)max[1],(float)min[2]);
    glVertex3f((float)max[0],(float)max[1],(float)max[2]);
    glVertex3f((float)min[0],(float)max[1],(float)min[2]);
    glVertex3f((float)min[0],(float)max[1],(float)max[2]);
    glEnd();
    glPopAttrib();        
}

bool render_bbox::isApplicable(Model *model){
    return !(model->bbox().isNull());
}

Q_EXPORT_PLUGIN(render_bbox)
