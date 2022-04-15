#pragma once

#include "commons/GeometryUtils.h"
#include "modules/geometry/TomoProjectionsSet.h"

#include <QVector>

class TomoProjections
{
public:
    TomoProjections() = default;

    TomoProjections( const Position3D & p_bottomLeftFront, const WSize3D & p_wsize, const Size3D & p_size );
    TomoProjections( const Position3D & p_bottomLeftFront, const WSize3D & p_wsize, const VoxelSpacing & p_voxelSpacing );
    TomoProjections( const Position3D & p_bottomLeftFront, const Size3D & p_size, const VoxelSpacing & p_voxelSpacing );
    TomoProjections( const Pave & p_pave, const Size3D & p_size );
    TomoProjections( const Pave & p_pave, const VoxelSpacing & p_voxelSpacing );


    void SetVoxelSpacingAndUpdateWSize( const VoxelSpacing & p_voxelSpacing );
    void SetVoxelSpacingAndUpdateSize( const VoxelSpacing & p_voxelSpacing );
    void SetSizeAndUpdateVoxelSpacing( const Size3D & p_size );
    void SetSizeAndUpdateWSize( const Size3D & p_size );
    void SetWSizeAndUpdateVoxelSpacing( const WSize3D & p_wsize );
    void SetWSizeAndUpdateSize( const WSize3D & p_wsize );

    TomoProjections & operator=( const TomoProjections & p_originalTomoProjections );

    bool Contains( const Position3D & p_point ) const;
    int GetVoxelsNumber() const;
    float GetSize() const;

    Size3D GetSize3D() const;
    WSize3D GetWSize3D() const;
    Position3D GetBLF() const;
    Position3D GetTRB() const;
    VoxelSpacing GetVoxelSpacing() const;

    // for computation optimizations
    QVector<float> GetXPositions() const;
    QVector<float> GetYPositions() const;
    QVector<float> GetZPositions() const;

    QVector<Position3D> GetGridPositions() const;
    QVector<int> GetVoxelsIndices() const;

private:
    void UpdatePositions();
    void UpdateWSize();
    void UpdateSize();
    void UpdateVoxelSpacing();

    Pave m_pave;
    Size3D m_size;
    VoxelSpacing m_voxelSpacing;

    // for computation optimizations
    QVector<float> m_xPositions;
    QVector<float> m_yPositions;
    QVector<float> m_zPositions;

    QVector<Position3D> m_gridPositions;
    QVector<int> m_voxelsIndices;
};
