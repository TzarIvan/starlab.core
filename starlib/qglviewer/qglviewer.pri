QT += opengl xml

DEPENDPATH  += qglviewer
INCLUDEPATH += qglviewer

# Is this needed inside?
DEFINES += QGLVIEWER_STATIC

# because we are not including vectorial headers
DEFINES += NO_VECTORIAL_RENDER

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
