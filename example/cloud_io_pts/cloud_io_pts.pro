CONFIG += starlab 
STARLAB_TEMPLATE = plugin 
STARLAB_DEPENDS += ../cloud

HEADERS += cloud_io_pts.h
SOURCES += cloud_io_pts.cpp

# Windows warnings
win32: DEFINES += _CRT_SECURE_NO_WARNINGS
