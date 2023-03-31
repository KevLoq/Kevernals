#pragma once

#include "modules/geometry/Pixel.h"
#include "modules/geometry/PixelOnProjection.h"
#include "modules/geometry/PixelSpacing.h"
#include "modules/geometry/Position2D.h"
#include "modules/geometry/Size2D.h"
#include "modules/geometry/WSize2D.h"

#include <vector>

class TomoProjectionsSet
{
public:
    TomoProjectionsSet() = default;
    ~TomoProjectionsSet() = default;

    TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const WSize2D & p_wsize, const Size2D & p_size );
    TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const WSize2D & p_wsize, const PixelSpacing & p_pixelSpacing );
    TomoProjectionsSet( const std::vector<Position2D> & p_bottomLeftpositions, const Size2D & p_size, const PixelSpacing & p_pixelSpacing );

    friend std::ostream & operator<<( std::ostream & p_outputStream, TomoProjectionsSet const & p_data );
    TomoProjectionsSet & operator=( const TomoProjectionsSet & p_originalTomoProjectionsSet );

    void SetPixelSpacingAndUpdateWSize( const PixelSpacing & p_pixelSpacing );
    void SetPixelSpacingAndUpdateSize( const PixelSpacing & p_pixelSpacing );
    void SetSizeAndUpdatePixelSpacing( const Size2D & p_size );
    void SetSizeAndUpdateWSize( const Size2D & p_size );
    void SetWSizeAndUpdatePixelSpacing( const WSize2D & p_wsize );
    void SetWSizeAndUpdateSize( const WSize2D & p_wsize );

    std::vector<Position2D> GetBottomLeftPositions() const;
    Size2D GetSize() const;
    WSize2D GetWSize() const;
    PixelSpacing GetPixelSpacing() const;
    std::vector<std::vector<float>> GetXPositions() const;
    std::vector<std::vector<float>> GetYPositions() const;
    std::vector<Position2D> GetProjectionPositions() const;
    std::vector<int> GetProjectionPixelsIndices() const;
    // std::vector<PixelOnProjection> GetProjectionIndiceAndPixels() const;
    int GetNProjections() const;

    Position2D GetPosition( int p_projectionIndex, Pixel p_pixelPosition ) const;
    Pixel GetPixel( int p_projectionIndex, Position2D p_pixelPosition ) const;

    void RemoveProjection( std::vector<int> const & p_dataIndicesToRemove );

    bool ContainsInside( const Position2D & p_point, int p_projectionIndex ) const;

private:
    void UpdatePositions();
    void UpdateWSize();
    void UpdateSize();
    void UpdatePixelSpacing();


    std::vector<Position2D> m_bottomLeftPositions;
    Size2D m_size{ 0, 0 };
    WSize2D m_wSize{ 0.F, 0.F };
    PixelSpacing m_pixelSpacing{ 0.F, 0.F };
    std::vector<std::vector<float>> m_xPositions;
    std::vector<std::vector<float>> m_yPositions;
    std::vector<Position2D> m_projectionPositions;
    // std::vector<PixelOnProjection> m_projectionIndiceAndPixels;
    std::vector<int> m_projectionPixelsIndices;
    int m_nProjections;
};


inline std::ostream & operator<<( std::ostream & p_outputStream, TomoProjectionsSet const & p_data )
{
    return p_outputStream << "nb Projections: " << p_data.m_nProjections << std::endl
                          << "size: " << p_data.m_wSize << " ; pixels size: " << p_data.m_size << " and pixels spacing: " << p_data.m_pixelSpacing << std::endl
                          << "bottom left position: " << p_data.m_bottomLeftPositions << std::endl;
}