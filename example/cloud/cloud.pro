load($$[STARLAB])
StarlabTemplate(sharedlib)
 
# Expose this to the global build
system(qmake -set CLOUD $$PWD/cloud.prf)
OTHER_FILES = cloud.prf

DEFINES += EXPORTFLAG

HEADERS += \
    CloudModel.h \
    CloudPlugins.h \
    global.h

SOURCES += \
    CloudModel.cpp

