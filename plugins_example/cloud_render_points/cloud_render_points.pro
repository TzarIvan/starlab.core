CONFIG += starlab 
STARLAB_TEMPLATE += plugin 
STARLAB_DEPENDS += ../cloud

QT += opengl

HEADERS = cloud_render_points.h
SOURCES = cloud_render_points.cpp
RESOURCES = resources.qrc
