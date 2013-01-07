TEMPLATE = subdirs
CONFIG += ordered

#--- COMPILES THE CORE
SUBDIRS += ../../starlab-core

#--- COMPILES THE BASIC EXAMPLES
SUBDIRS += example_mode_withwidget

#--- AN EXAMPLE OF A BASIC DATA STRUCTURE
#--- AND A SET OF SIMPLE PLUGINS USING IT
SUBDIRS += cloud #< "cloud" model
SUBDIRS += cloud_io_pts
SUBDIRS += cloud_render_points
SUBDIRS += cloud_filter_normalize
SUBDIRS += cloud_mode_select
SUBDIRS += cloud_decorate_selection
