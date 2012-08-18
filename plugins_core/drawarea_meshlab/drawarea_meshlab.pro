# This is a renderer along the lines of the one in Meshlab. 
# Note the code internally is a mess... 
CONFIG += starlab
STARLAB_TEMPLATE = plugin

QT += opengl xml

HEADERS += drawarea_meshlab.h
SOURCES += drawarea_meshlab.cpp

HEADERS += eigen_2_vcg.h
HEADERS += GLArea.h
SOURCES += GLArea.cpp

HEADERS += vcglib/wrap/gui/trackball.h
HEADERS += vcglib/wrap/gui/trackmode.h
SOURCES += vcglib/wrap/gui/trackball.cpp
SOURCES += vcglib/wrap/gui/trackmode.cpp

INCLUDEPATH+=vcglib
DEPENDPATH+=vcglib

#--- GLEW STUFF
GLEWDIR = $$PWD/glew-1.5.1
DEFINES += GLEW_STATIC
INCLUDEPATH += $$GLEWDIR/include
SOURCES += $$GLEWDIR/src/glew.c






