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
SUBDIRS += starlab
SUBDIRS += star_term  #(Qt Creator: last in alphabetical order is run by default)

#--- GUI MODULES
SUBDIRS += plugins/project_io_starlab
SUBDIRS += plugins/render_bbox
SUBDIRS += plugins/gui_filemenu
SUBDIRS += plugins/gui_filter
SUBDIRS += plugins/gui_mode
SUBDIRS += plugins/gui_selection
SUBDIRS += plugins/gui_windows
SUBDIRS += plugins/gui_view

#--- EXAMPLES
SUBDIRS += example/cloud
SUBDIRS += example/cloud_io_pts
SUBDIRS += example/cloud_filter_normalize
SUBDIRS += example/cloud_mode_selection
SUBDIRS += example/cloud_render_points

