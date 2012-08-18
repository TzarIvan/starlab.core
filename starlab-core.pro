system(qmake -set QMAKEFEATURES $$PWD) #QT BUG
system(qmake -set BUILD_PATH $$OUT_PWD) #for Windows, any better way?

TEMPLATE = subdirs
CONFIG += ordered 

#--- QUICK LINKS TO EXTERNALS CONFIGS
OTHER_FILES += external/eigen/eigen.prf
OTHER_FILES += external/qhull/qhull.prf
OTHER_FILES += starlab.prf README.txt

#--- CORE LIBRARIES 
SUBDIRS += libraries/parameters
SUBDIRS += libraries/common

#--- APPLICATION MAKER
SUBDIRS += maingui

#--- GUI MODULES
SUBDIRS += plugins_core/drawarea_meshlab
SUBDIRS += plugins_core/drawarea_starlab
SUBDIRS += plugins_core/render_bbox
SUBDIRS += plugins_core/gui_filemenu
SUBDIRS += plugins_core/gui_filter
SUBDIRS += plugins_core/gui_mode
SUBDIRS += plugins_core/gui_selection
SUBDIRS += plugins_core/gui_windows
SUBDIRS += plugins_core/gui_view

#--- EXAMPLES
SUBDIRS += plugins_example
