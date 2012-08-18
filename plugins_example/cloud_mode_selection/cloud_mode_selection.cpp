#include "cloud_mode_selection.h"
#include <qgl.h>

#if 0
void cloud_mode_selection::DrawXORRect(){  

  bool doubleDraw = false;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,gla->curSiz.width(),gla->curSiz.height(),0,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_LOGIC_OP);
  glLogicOp(GL_XOR);
  glColor3f(1,1,1);
  if(doubleDraw)
  {
    glBegin(GL_LINE_LOOP);
      glVertex2f(start.x(),start.y());
      glVertex2f(prev.x(),start.y());
      glVertex2f(prev.x(),prev.y());
      glVertex2f(start.x(),prev.y());
    glEnd();
  }
  glBegin(GL_LINE_LOOP);
    glVertex2f(start.x(),start.y());
    glVertex2f(cur.x(),start.y());
    glVertex2f(cur.x(),cur.y());
    glVertex2f(start.x(),cur.y());
  glEnd();
  glDisable(GL_LOGIC_OP);
  
  // Closing 2D
  glPopAttrib();
    glPopMatrix(); // restore modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
#endif

Q_EXPORT_PLUGIN(cloud_mode_selection)
