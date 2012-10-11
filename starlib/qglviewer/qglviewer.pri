QT += opengl xml

DEPENDPATH  += qglviewer
INCLUDEPATH += qglviewer

# Is this needed inside?
DEFINES += QGLVIEWER_STATIC

HEADERS *= \
    qglviewer/qglviewer.h \
    qglviewer/camera.h \
    qglviewer/manipulatedFrame.h \
    qglviewer/manipulatedCameraFrame.h \
    qglviewer/frame.h \
    qglviewer/constraint.h \
    qglviewer/keyFrameInterpolator.h \
    qglviewer/mouseGrabber.h \
    qglviewer/quaternion.h \
    qglviewer/vec.h \
    qglviewer/domUtils.h \
    qglviewer/config.h

SOURCES *= \
    qglviewer/qglviewer.cpp \
    qglviewer/camera.cpp \
    qglviewer/manipulatedFrame.cpp \
    qglviewer/manipulatedCameraFrame.cpp \
    qglviewer/frame.cpp \
    qglviewer/saveSnapshot.cpp \
    qglviewer/constraint.cpp \
    qglviewer/keyFrameInterpolator.cpp \
    qglviewer/mouseGrabber.cpp \
    qglviewer/quaternion.cpp \
    qglviewer/vec.cpp

TRANSLATIONS = \
    qglviewer/qglviewer_fr.ts

# -----------------------------------
# --  I m a g e I n t e r f a c e  --
# -----------------------------------
contains( QT_VERSION, "^4.*" ) {
  FORMS *= qglviewer/ImageInterface.Qt4.ui
} else {
  FORMS *= qglviewer/ImageInterface.Qt3.ui
}

# ---------------------------------------------
# --  V e c t o r i a l   R e n d e r i n g  --
# ---------------------------------------------
# In case of compilation troubles with vectorial rendering, uncomment this line
# DEFINES *= NO_VECTORIAL_RENDER

contains( DEFINES, NO_VECTORIAL_RENDER ) {
  message( Vectorial rendering disabled )
} else {
  contains( QT_VERSION, "^4.*" ) {
    FORMS *= qglviewer/VRenderInterface.Qt4.ui
  } else {
    FORMS *= qglviewer/VRenderInterface.Qt3.ui
  }

  SOURCES *= \
	qglviewer/VRender/BackFaceCullingOptimizer.cpp \
	qglviewer/VRender/BSPSortMethod.cpp \
	qglviewer/VRender/EPSExporter.cpp \
	qglviewer/VRender/Exporter.cpp \
	qglviewer/VRender/FIGExporter.cpp \
	qglviewer/VRender/gpc.cpp \
	qglviewer/VRender/ParserGL.cpp \
	qglviewer/VRender/Primitive.cpp \
	qglviewer/VRender/PrimitivePositioning.cpp \
	qglviewer/VRender/TopologicalSortMethod.cpp \
	qglviewer/VRender/VisibilityOptimizer.cpp \
	qglviewer/VRender/Vector2.cpp \
	qglviewer/VRender/Vector3.cpp \
	qglviewer/VRender/NVector3.cpp \
	qglviewer/VRender/VRender.cpp

  HEADERS *= \
	qglviewer/VRender/AxisAlignedBox.h \
	qglviewer/VRender/Exporter.h \
	qglviewer/VRender/gpc.h \
	qglviewer/VRender/NVector3.h \
	qglviewer/VRender/Optimizer.h \
	qglviewer/VRender/ParserGL.h \
	qglviewer/VRender/Primitive.h \
	qglviewer/VRender/PrimitivePositioning.h \
	qglviewer/VRender/SortMethod.h \
	qglviewer/VRender/Types.h \
	qglviewer/VRender/Vector2.h \
	qglviewer/VRender/Vector3.h \
	qglviewer/VRender/VRender.h
}
