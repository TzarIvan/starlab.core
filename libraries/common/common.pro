CONFIG += starlab
STARLAB_TEMPLATE = dynamic
STARLAB_DEPENDS += parameters 
STARLAB_EXTERNAL += qglviewer

#SetupStarlabLibrary(dynamic)
#ImportStarlabLibrary(dynamic_parameters)

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
    OpenGLErrorChecker.h \

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

