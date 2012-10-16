#----------------------- THIS FILE SHOULD NOT BE MODIFIED ----------------------
#
# It is the one that is used to do "SUBDIRS += starlab-core". For this reason 
# the examples are not included here. To compile the core as a standalone demo 
# with examples, please see "starlab-core-examples.pro"
#-------------------------------------------------------------------------------
TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += starlib   #< SHARED LIBRARY
SUBDIRS += starterm  #< TERMINAL APPLICATION
SUBDIRS += starlab   #< GUI APPLICATION

#--- LIST CORE FEATURES
OTHER_FILES += starlab.prf              #< starlab core features
OTHER_FILES += external/qhull.prf       #< StarlabExternal(qhull)
OTHER_FILES += external/eigen.prf       #< StarlabExternal(eigen)
OTHER_FILES += external/cholmod.prf     #< StarlabExternal(cholmod)

#--- CORE PLUGINS
SUBDIRS += plugins/render_bbox
SUBDIRS += plugins/gui_filemenu
SUBDIRS += plugins/gui_filter
SUBDIRS += plugins/gui_mode
SUBDIRS += plugins/gui_render
SUBDIRS += plugins/gui_decorate
SUBDIRS += plugins/gui_windows
SUBDIRS += plugins/gui_view
SUBDIRS += plugins/project_io_starlab
