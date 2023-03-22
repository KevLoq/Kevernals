#pragma once

#include "modules/geometry/Dim3.h"

struct VoxelSpacing;
struct WSize3D;
struct Size3D : public Dim3<int>
{  
    Size3D(int p_x,int p_y,int p_z): Dim3<int>(p_x,p_y,p_z) {}
    ~Size3D() override = default;
    static Size3D FromVoxelSpacingAndSizeInWorld( const VoxelSpacing & p_voxelSpacing, const WSize3D & p_sizeInWorld, bool & p_allDimensionsValid );
};
