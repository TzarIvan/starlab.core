load($$PWD/../../starlab.prf)
StarlabTemplate(sharedlib)

DEFINES += EXPORTFLAG

HEADERS += \
    CloudModel.h \
    CloudPlugins.h \
    global.h

SOURCES += \
    CloudModel.cpp

