#pragma once

#include "commons/GeometryUtils.h"

#include <optional>
#include <vector>

enum class Side : int
{
    Bottom = 0,    // z max
    Top,           // z min
    Left,          // y max
    Right,         // y min
    Front,         // x max
    Back           // x min
};


class TomoVolume
{
public:
    TomoVolume() = default;
    ~TomoVolume() = default;

    TomoVolume( const Position3D & p_bottomLeftFront, const WSize3D & p_wsize, const Size3D & p_size );
    TomoVolume( const Position3D & p_bottomLeftFront, const WSize3D & p_wsize, const VoxelSpacing & p_voxelSpacing );
    TomoVolume( const Position3D & p_bottomLeftFront, const Size3D & p_size, const VoxelSpacing & p_voxelSpacing );
    TomoVolume( const Pave & p_pave, const Size3D & p_size );
    TomoVolume( const Pave & p_pave, const VoxelSpacing & p_voxelSpacing );


    void SetVoxelSpacingAndUpdateWSize( const VoxelSpacing & p_voxelSpacing );
    void SetVoxelSpacingAndUpdateSize( const VoxelSpacing & p_voxelSpacing );
    void SetSizeAndUpdateVoxelSpacing( const Size3D & p_size );
    void SetSizeAndUpdateWSize( const Size3D & p_size );
    void SetWSizeAndUpdateVoxelSpacing( const WSize3D & p_wsize );
    void SetWSizeAndUpdateSize( const WSize3D & p_wsize );

    std::vector<Position3D> FindIntersectionPointsWithLine( const Position3D & p_origin, const Vector3D & p_rayVector ) const;
    TomoVolume & operator=( const TomoVolume & p_originalTomoVolume );

    bool Contains( const Position3D & p_point ) const;
    bool ContainsInsideVoxelGrid( const Position3D & p_point ) const;
    int GetVoxelsNumber() const;
    float GetSize() const;

    Size3D GetSize3D() const;
    WSize3D GetWSize3D() const;
    Position3D GetBLF() const;
    Position3D GetTRB() const;
    VoxelSpacing GetVoxelSpacing() const;

    // for computation optimizations
    std::vector<float> GetXPositions() const;
    std::vector<float> GetYPositions() const;
    std::vector<float> GetZPositions() const;

    std::vector<Position3D> GetGridPositions() const;
    std::vector<int> GetVoxelsIndices() const;

    Voxel FindVoxelContainingPosition( const Position3D & p_position ) const;

private:
    void UpdatePositions();
    void UpdateWSize();
    void UpdateSize();
    void UpdateVoxelSpacing();


    Pave m_pave;
    Size3D m_size;
    VoxelSpacing m_voxelSpacing;

    // for computation optimizations
    std::vector<float> m_xPositions; 
    std::vector<float> m_yPositions;
    std::vector<float> m_zPositions;

    std::vector<Position3D> m_gridPositions;
    std::vector<int> m_voxelsIndices;
};
