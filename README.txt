TODO
+ Instead of passing NULL as 2nd argument of filters, the default StarlabDrawArea is a dummy which does nothing graphical
+ add virtual destructors to models
+ rename dynamic_surfacemesh into dynamic_model_surfacemesh
+ remove all references in favor of TYPE* const varname;
+ rename SUBDIRS += starlab into core (target= sets applicative name)
+ elementary plugin development????
  only do -I something, -L something
- reload GUI layer dialog after loading another model
- Use SAME keyboard controls as blender (perhaps keyboard control modes as plugins?)
- Export part of Surface_mesh
- Default cloud color to white..
- Settings edit GUI

COMMENTS FOR MARIO BOTSCH
- Surface_mesh => SurfaceMesh
- GL render routines in a subfolder of SurfaceMesh
- IO stuff in a subfolder of SurfaceMesh

INTERNAL NOTE
any plugin in the system beside "gui_" plugins strictly
has one action associated with it. In this way there is a 
strict relationship between [ action <==> gui element ]


IMPORTANT NOTE!!!!
At least one plugin needs to be compiled. Otherwise you will
end up not having a "Plugin" directory and running will result
in a crash!!

TODO for VS (Windows) 10/11/2011 - Ibraheem:
1) depdency order 
2) making 'meshlab' the default project in solution 
   ==> ATA2: you cannot just make it the first entry. 
             otherwise you break the dependencies!!
3) including '.' in paths
4) default path for 'meshlab.exe'



REMINDERS:
    use Q_ASSERT


The point of passing everything throgh parameters in plugins is that 
there is only one loaded plugin instance in the whole system. Consequently, 
if the plugin need to operate on different entities

