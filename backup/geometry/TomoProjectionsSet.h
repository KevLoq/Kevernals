#pragma once

#include "commons/GeometryUtils.h"

#include <vector>

class TomoProjectionsSet
{
public:
    TomoProjectionsSet() = default;
    ~TomoProjectionsSet() = default;

    TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const WSize2D & p_wsize, const Size2D & p_size );
    TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const WSize2D & p_wsize, const PixelSpacing & p_pixelSpacing );
    TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const Size2D & p_size, const PixelSpacing & p_pixelSpacing );

    TomoProjectionsSet & operator=( const TomoProjectionsSet & p_originalTomoProjectionsSet );

    void SetPixelSpacingAndUpdateWSize( const PixelSpacing & p_pixelSpacing );
    void SetPixelSpacingAndUpdateSize( const PixelSpacing & p_pixelSpacing );
    void SetSizeAndUpdatePixelSpacing( const Size2D & p_size );
    void SetSizeAndUpdateWSize( const Size2D & p_size );
    void SetWSizeAndUpdatePixelSpacing( const WSize2D & p_wsize );
    void SetWSizeAndUpdateSize( const WSize2D & p_wsize );

    const std::vector<Position2D> * GetBottomLeftPositionsPtr() const;
    Size2D GetSize() const;
    WSize2D GetWSize() const;
    PixelSpacing GetPixelSpacing() const;
    const std::vector<std::vector<float>> * GetXPositionsPtr() const;
    const std::vector<std::vector<float>> * GetYPositionsPtr() const;
    const std::vector<Position2D> * GetProjectionPositionsPtr() const;
    const std::vector<int> * GetProjectionPixelsIndicesPtr() const;
    const std::vector<PixelOnProjection> * GetProjectionIndiceAndPixelsPtr() const;
    int GetNProjections() const;

    Position2D GetPosition( int p_projectionIndex, Pixel p_pixelPosition ) const;
    Pixel GetPixel( int p_projectionIndex, Position2D p_pixelPosition ) const;

    bool ContainsInside( const Position2D & p_point, int p_projectionIndex ) const;

private:
    void UpdatePositions();
    void UpdateWSize();
    void UpdateSize();
    void UpdatePixelSpacing();


    std::vector<Position2D> m_bottomLeftPositions;
    Size2D m_size;
    WSize2D m_wSize;
    PixelSpacing m_pixelSpacing;
    std::vector<std::vector<float>> m_xPositions;
    std::vector<std::vector<float>> m_yPositions;
    std::vector<Position2D> m_projectionPositions;
    std::vector<PixelOnProjection> m_projectionIndiceAndPixels;
    std::vector<int> m_projectionPixelsIndices;
    int m_nProjections;
};
