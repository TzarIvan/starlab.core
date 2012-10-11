CONFIG += starlab
STARLAB_TEMPLATE = console
STARLAB_DEPENDS = starlib

HEADERS += \
    QCommandLine.h \
    CmdLineParser.h

SOURCES += main.cpp \
    QCommandLine.cpp \
    CmdLineParser.cpp
