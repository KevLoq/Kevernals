#include "modules/geometry/WSize3D.h"

#include "modules/geometry/Size3D.h"
#include "modules/geometry/VoxelSpacing.h"


WSize3D::WSize3D( const VoxelSpacing & p_voxelSpacing, const Size3D & p_size )
 : Dim3<float>(static_cast<float>( p_size.x ) * p_voxelSpacing.x,static_cast<float>( p_size.y ) * p_voxelSpacing.y,static_cast<float>( p_size.z ) * p_voxelSpacing.z) 
{} 
// Why constructor is located here and not in header file.
// Perfect forwrding prevents from include cycles betwee WSize3D, Size3D and VoxelSpacing classes.
// But does not allow for the use of objects of the classes Size3D and VoxelSpacing in header file.