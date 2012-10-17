load($$PWD/../starlab.prf)
StarlabTemplate(console)

TARGET = starlab

HEADERS += \
    QCommandLine.h \
    CmdLineParser.h

SOURCES += main.cpp \
    QCommandLine.cpp \
    CmdLineParser.cpp
