CONFIG += starlab
STARLAB_TEMPLATE = console
STARLAB_DEPENDS += common parameters

SOURCES += main.cpp \
    CmdLineParser.cpp

HEADERS += \
    CoreApplication.h \
    CmdLineParser.h
