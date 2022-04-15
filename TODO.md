# Buld and Debug environment

1. Make Altumira lib optional (as it is commercial lib) and use it only if present
2. Fix the launch.json : PATH to be set from settings.json or from CMake variables
3. Fix the problem of #include marked as failing in VS Code intellisense
   
# Dev

1. Projection interface :
   1. Remove the unnecessary embedding classes VolumeData and ProjectionData (mainly used in Projector(Cuda))
   2. Migrate the RayTracingProjection in projection Dir and adapt IProjector interface for it
   3. Rehabilitate Shift and Add projectors
