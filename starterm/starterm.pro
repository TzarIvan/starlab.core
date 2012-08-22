CONFIG += starlab
STARLAB_TEMPLATE = console
STARLAB_DEPENDS += common parameters

SOURCES += main.cpp \
    CmdLineParser.cpp

HEADERS += \
    CmdLineParser.h
