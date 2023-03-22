#include "modules/geometry/Voxel.h"
 
float Voxel::DistanceL2To( const Voxel & p_point ) const
{
    auto diffX = x - p_point.x;
    auto diffY = y - p_point.y;
    auto diffZ = z - p_point.z;
    return std::sqrt( diffX * diffX + diffY * diffY + diffZ * diffZ );
}     

int Voxel::DistanceL1To( const Voxel & p_point ) const
{
    auto diffX = x - p_point.x;
    auto diffY = y - p_point.y;
    auto diffZ = z - p_point.z;
    return std::abs( diffX) + std::abs( diffY ) + std::abs( diffZ );
}
