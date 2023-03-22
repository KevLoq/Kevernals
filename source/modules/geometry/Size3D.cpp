#include "modules/geometry/Size3D.h"

#include "modules/geometry/VoxelSpacing.h"
#include "modules/geometry/WSize3D.h"

Size3D Size3D::FromVoxelSpacingAndSizeInWorld( const VoxelSpacing & p_voxelSpacing, const WSize3D & p_sizeInWorld, bool & p_allDimensionsValid )
{
    p_allDimensionsValid = true;
    Size3D size(0,0,0);
    if( p_voxelSpacing.x > 0.F )
    {
        size.x = static_cast<int>( p_sizeInWorld.x / p_voxelSpacing.x );
    }
    else
    {
        p_allDimensionsValid = false;
    }
    if( p_voxelSpacing.y > 0.F )
    {
        size.y = static_cast<int>( p_sizeInWorld.y / p_voxelSpacing.y );
    }
    else
    {
        p_allDimensionsValid = false;
    }
    if( p_voxelSpacing.z > 0.F )
    {
        size.z = static_cast<int>( p_sizeInWorld.z / p_voxelSpacing.z );
    }
    else
    {
        p_allDimensionsValid = false;
    }
    return size;
}