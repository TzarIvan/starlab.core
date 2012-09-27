#include <QMouseEvent>
#include "cloud_mode_select.h"
#include "StarlabDrawArea.h"
#include <qgl.h>

bool cloud_mode_select::mousePressEvent(QMouseEvent* event){
    // qDebug() << "cloud_mode_select::mousePressEvent()";

    selectionMode = ADD;
    drawArea()->setMouseBinding(Qt::LeftButton, QGLViewer::SELECT);

    // Clear selection
    selection.clear();
    cloud()->property["selected"] = QVector<QVariant>(cloud()->points.size());

    start = event->pos();
    cur = start;
    return true;
}

bool cloud_mode_select::mouseMoveEvent(QMouseEvent* event){
    // qDebug() << "cloud_mode_select::mouseMoveEvent()";

    if(selectionMode == NONE) return true;

    /// @todo update the selection at runtime
    prev = cur;
    cur = event->pos();
    drawArea()->updateGL();
    return true;
}

bool cloud_mode_select::mouseReleaseEvent(QMouseEvent *){
    if (selectionMode != NONE)
    {
        QRect rectangle(start, cur);
        rectangle = rectangle.normalized();

        // Define selection window dimensions
        drawArea()->setSelectRegionWidth(rectangle.width());
        drawArea()->setSelectRegionHeight(rectangle.height());

        // Compute rectangle center and perform selection
        drawArea()->select(rectangle.center());
    }

    drawArea()->updateGL();
    return true;
}

void cloud_mode_select::endSelection(const QPoint &)
{
    // Flush GL buffers
    glFlush();

    // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
    GLint nbHits = glRenderMode(GL_RENDER);

    if (nbHits > 0){
        // Interpret results : each object created 4 values in the selectBuffer().
        // (selectBuffer())[4*i+3] is the id pushed on the stack.
        for (int i=0; i<nbHits; ++i)
            switch (selectionMode)
            {
            case ADD    : selection.insert((drawArea()->selectBuffer())[4*i+3]); break;
            case REMOVE : selection.remove((drawArea()->selectBuffer())[4*i+3]);  break;
            default : break;
            }
    }

    // End selection
    selectionMode = NONE;

    // Handle selection
    QVector<QVariant> vselect(cloud()->points.size());

    foreach(int idx, selection)
        vselect[idx] = true;

    cloud()->property["selected"] = vselect;

    /// @todo create StarlabPlugin::log() to publish log messages
    // mainWindow()->statusBarMessage(QString("( %1 ) points selected.").arg(nbHits));
}

void cloud_mode_select::decorate(){
    // qDebug() << "cloud_mode_select::paintEvent";

    if (selectionMode != NONE)
        drawSelectionRectangle();

    // Draw selected items
    drawArea()->qglColor(QColor(255,0,0));
    glPointSize(10);

    QVector<QVariant> vselect = cloud()->getProperty("selected");

    glBegin(GL_POINTS);
    for(int i = 0; i < vselect.size(); i++){
        if(vselect[i] == true){
            QVector4D v = cloud()->points[i];
            glVertex3d(v.x(), v.y(), v.z());
        }
    }
    glEnd();
}

void cloud_mode_select::drawWithNames(){
    //qDebug() << "cloud_mode_select::drawWithNames()";

    int i = 0;

    foreach(QVector4D v, cloud()->points){
        glPushName(i++);
        glBegin(GL_POINTS);
            glVertex3d(v.x(), v.y(), v.z());
        glEnd();
        glPopName();
    }
}

void cloud_mode_select::drawSelectionRectangle(){  
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT );
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        int width = drawArea()->width();
        int height = drawArea()->height();
        glOrtho(0,width,height,0,-1,1);
            
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor4d(1.0, 0.0, 0.0, 0.2);
        glBegin(GL_LINE_STRIP);
            glVertex2f(start.x(),start.y());
            glVertex2f(cur.x(),start.y());
            glVertex2f(cur.x(),cur.y());
            glVertex2f(start.x(),cur.y());
            glVertex2f(start.x(),start.y());
        glEnd();
    glPopAttrib();
}


Q_EXPORT_PLUGIN(cloud_mode_select)
