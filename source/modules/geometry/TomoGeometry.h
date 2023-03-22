#pragma once

#include "modules/geometry/Pixel.h"
#include "modules/geometry/PixelOnProjection.h"
#include "modules/geometry/PixelSpacing.h"
#include "modules/geometry/Position2D.h"
#include "modules/geometry/Position3D.h"
#include "modules/geometry/Size2D.h"
#include "modules/geometry/Size3D.h"
#include "modules/geometry/TomoProjectionsSet.h"
#include "modules/geometry/TomoTable.h"
#include "modules/geometry/TomoVolume.h"
#include "modules/geometry/VoxelSpacing.h"
#include "modules/geometry/WSize2D.h"
#include "modules/geometry/WSize3D.h"

#include <memory>
#include <string>
#include <vector>

class TomoGeometry
{
public:
    TomoGeometry( const std::string & p_xmlGeometryFilePath );
    ~TomoGeometry() = default;

    friend std::ostream & operator<<( std::ostream & p_outputStream, TomoGeometry const & p_data );
    bool IsValid() const { return m_isValid; }

    // accessors
    std::string filePath() const { return m_filePath; }

    // sources geometric features
    std::vector<float> sourcesYPositions() const;
    float sourcesXCommonPosition() const;
    float sourcesZCommonPosition() const;

    float sid() const;
    float detectorsZCommonPosition() const;

    // detectors geometric features
    int nbProjections() const;
    std::vector<Position2D> projectionsBottomLeftPositions() const;
    std::vector<Position2D> projectionsPositions() const;
    Size2D projectionsSize() const;
    WSize2D projectionsWSize() const;
    std::vector<std::vector<float>> projectionsXPositions() const;
    std::vector<std::vector<float>> projectionsYPositions() const;
    PixelSpacing projectionsPixelSpacing() const;

    // mainly used for parallelization
    std::vector<int> projectionsPositionsIndices() const;
    std::vector<PixelOnProjection> projectionIndiceAndPixels() const;

    int nbProjectionsRois() const;
    std::vector<Position2D> projectionsRoisBottomLeftPositions() const;
    std::vector<Position2D> projectionsRoisPositions() const;
    Size2D projectionsRoisSize() const;
    WSize2D projectionsRoisWSize() const;
    std::vector<std::vector<float>> projectionsRoisXPositions() const;
    std::vector<std::vector<float>> projectionsRoisYPositions() const;
    PixelSpacing projectionsRoisPixelSpacing() const;

    // mainly used for parallelization
    std::vector<int> projectionsRoisPositionsIndices() const;
    std::vector<PixelOnProjection> projectionRoisIndiceAndPixels() const;

    // volume geometric features
    Size3D volumeSize3D() const;
    float volumeSize() const;
    VoxelSpacing volumeVoxelSpacing() const;
    WSize3D volumeWSize3D() const;
    Position3D volumeBLF() const;
    Position3D volumeTRB() const;
    std::vector<float> volumeZs() const;
    std::vector<float> volumeYs() const;
    std::vector<float> volumeXs() const;
    std::vector<Position3D> volumeGridPositions() const;

    WSize3D fulcrum() const { return m_fulcrum; }
    WSize3D rotationCenter() const { return m_fulcrum; }

    // mainly used for parallelization
    std::vector<int> volumeVoxelsIndices() const;

    TomoVolume * GetVolume() const { return m_volume.get(); }
    TomoProjectionsSet * GetProjections() const { return m_projections.get(); }
    TomoProjectionsSet * GetProjectionsRois() const { return m_projectionsRois.get(); }
    TomoTable * GetTable() const { return m_table.get(); }

    Pixel GetProjectionROIsBLPixelPositionOnDetector() const { return m_projectionROIsBLPixelPositionOnDetector; }

private:
    std::string m_filePath;    // OK

    // volume geometric features
    std::unique_ptr<TomoVolume> m_volume;
    std::unique_ptr<TomoProjectionsSet> m_projections;
    std::unique_ptr<TomoProjectionsSet> m_projectionsRois;
    std::unique_ptr<TomoTable> m_table;
    Pixel m_projectionROIsBLPixelPositionOnDetector{ 0, 0 };
    WSize3D m_fulcrum{ 0.F, 0.F, 0.F };

    bool m_isValid{ false };
};

inline bool operator==( TomoGeometry const & p_tomoGeometryA, TomoGeometry const & p_tomoGeometryB )
{
    return p_tomoGeometryA.filePath() == p_tomoGeometryB.filePath();
}

inline bool operator!=( TomoGeometry const & p_tomoGeometryA, TomoGeometry const & p_tomoGeometryB )
{
    return !( p_tomoGeometryA == p_tomoGeometryB );
}

inline std::ostream & operator<<( std::ostream & p_outputStream, TomoGeometry const & p_data )
{
    return p_outputStream << "volume " << *p_data.m_volume << std::endl
                          << "projections: " << *p_data.m_projections << std::endl
                          << "projections Rois: " << *p_data.m_projectionsRois << std::endl
                          << "table: " << *p_data.m_table << std::endl;
}
