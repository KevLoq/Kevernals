#pragma once

#include "modules/geometry/Dim3.h"

struct Voxel : public Dim3<int>
{ 
    Voxel(int p_x,int p_y,int p_z): Dim3<int>(p_x,p_y,p_z) {} ;
    ~Voxel() override = default;
    int DistanceL1To( const Voxel & p_point ) const;
    float DistanceL2To( const Voxel & p_point ) const; 
};
