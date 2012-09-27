system(qmake -set QMAKEFEATURES $$PWD) # QT-BUG

TEMPLATE = subdirs
CONFIG += ordered
OTHER_FILES += starlab.prf

SUBDIRS += libraries/parameters
SUBDIRS += libraries/common
SUBDIRS += starterm
SUBDIRS += starlab

# CORE PLUGINS
SUBDIRS += plugins/render_bbox
SUBDIRS += plugins/gui_filemenu
SUBDIRS += plugins/gui_filter
SUBDIRS += plugins/gui_mode
SUBDIRS += plugins/gui_render
SUBDIRS += plugins/gui_decorate
SUBDIRS += plugins/gui_windows
SUBDIRS += plugins/gui_view
SUBDIRS += plugins/project_io_starlab
 
## EXAMPLE PLUGINS (cloud)
SUBDIRS += example/cloud
SUBDIRS += example/cloud_io_pts
SUBDIRS += example/cloud_mode_select
SUBDIRS += example/cloud_render_points
SUBDIRS += example/cloud_filter_normalize

## EXAMPLE PLUGINS (basic)
SUBDIRS += example/example_mode_withwidget
