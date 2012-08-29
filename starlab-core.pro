system(qmake -set QMAKEFEATURES $$PWD) # QT-BUG

TEMPLATE = subdirs
CONFIG += ordered
OTHER_FILES += starlab.prf

SUBDIRS += libraries/parameters
SUBDIRS += libraries/common
SUBDIRS += starterm
SUBDIRS += starlab

SUBDIRS += plugins/render_bbox
SUBDIRS += plugins/gui_filemenu
SUBDIRS += plugins/gui_filter
SUBDIRS += plugins/gui_mode
SUBDIRS += plugins/gui_selection
SUBDIRS += plugins/gui_windows
SUBDIRS += plugins/gui_view
SUBDIRS += plugins/project_io_starlab
