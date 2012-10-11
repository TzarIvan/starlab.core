CONFIG += starlab
STARLAB_TEMPLATE = dynamic
STARLAB_DEPENDS = starlib

# Dependencies
QT += xml opengl
DEFINES += EXPORTFLAG
OTHER_FILES += *.prf

HEADERS += \
    CloudModel.h \
    CloudPlugins.h \
    global.h

SOURCES += \
    CloudModel.cpp

