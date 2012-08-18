CONFIG += starlab
STARLAB_TEMPLATE = dynamic
STARLAB_DEPENDS += parameters 

#SetupStarlabLibrary(dynamic)
#ImportStarlabLibrary(dynamic_parameters)

# Needed to define DrawArea
QT *= opengl

#DEFINES += DEBUG_PLUGINMANAGER
#DEFINES += DEBUG_STARAPP

include(qt3d/qt3d.pri)
include(QCommandLine/QCommandLine.pri)
include(interfaces/interfaces.pri)

HEADERS += \
    dynamic_common_global.h \
    Document.h \
    Callbacks.h \
    StarlabSettings.h \
    StarlabException.h \
    StarlabApplication.h \
    PluginManager.h \
    Model.h \
    Logger.h \
    StarlabDrawArea.h \
    StarlabMainWindow.h \
    RenderObject.h \
    OSQuery.h \
    OpenGLErrorChecker.h


SOURCES += \
    Document.cpp \
    StarlabSettings.cpp \
    PluginManager.cpp \
    Model.cpp \
    StarlabApplication.cpp \
    StarlabMainWindow.cpp \
    StarlabDrawArea.cpp \
    RenderObject.cpp \

RESOURCES +=

win32{
    DEFINES += Q_QT3D
    DEFINES += DYNAMIC_COMMON
}

