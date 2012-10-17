TEMPLATE = subdirs
CONFIG += ordered

#--- COMPILES THE CORE
SUBDIRS += ../../starlab-core

#--- COMPILES THE EXAMPLES
SUBDIRS += cloud
SUBDIRS += cloud_io_pts
SUBDIRS += cloud_render_points
SUBDIRS += cloud_filter_normalize
SUBDIRS += cloud_mode_select
SUBDIRS += cloud_decorate_selection
SUBDIRS += example_mode_withwidget
