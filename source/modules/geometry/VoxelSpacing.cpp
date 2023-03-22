#include "modules/geometry/VoxelSpacing.h"

#include "modules/geometry/Size3D.h"
#include "modules/geometry/WSize3D.h"

VoxelSpacing VoxelSpacing::FromSizes( const Size3D & p_dimensions, const WSize3D & p_sizeInWorld, bool & p_allDimensionsValid )
{
    p_allDimensionsValid = true;
    
    VoxelSpacing voxelSpacing{1.F, 1.F, 1.F};
    if( p_dimensions.x > 0 )
    {
        voxelSpacing.x = p_sizeInWorld.x / static_cast<float>( p_dimensions.x );
    }
    else {
        p_allDimensionsValid = false;
    }
    if( p_dimensions.y > 0 )
    {
        voxelSpacing.y = p_sizeInWorld.y / static_cast<float>( p_dimensions.y );
    }
    else {
        p_allDimensionsValid = false;
    }
    if( p_dimensions.z > 0 )
    {
        voxelSpacing.z = p_sizeInWorld.z / static_cast<float>( p_dimensions.z );
    }
    else {
        p_allDimensionsValid = false;
    }
    return voxelSpacing;
}
