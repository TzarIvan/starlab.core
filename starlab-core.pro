TEMPLATE = subdirs
CONFIG += ordered
OTHER_FILES += starlab.prf

SUBDIRS += libraries/parameters
SUBDIRS += libraries/common
SUBDIRS += starlab
SUBDIRS += starterm
SUBDIRS += plugins_core/render_bbox
SUBDIRS += plugins_core/gui_filemenu
SUBDIRS += plugins_core/gui_filter
SUBDIRS += plugins_core/gui_mode
SUBDIRS += plugins_core/gui_selection
SUBDIRS += plugins_core/gui_windows
SUBDIRS += plugins_core/gui_view
SUBDIRS += plugins_core/project_io_starlab
