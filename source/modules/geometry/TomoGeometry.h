#pragma once

#include "commons/GeometryUtils.h"
#include "modules/geometry/TomoProjectionsSet.h"
#include "modules/geometry/TomoTable.h"
#include "modules/geometry/TomoVolume.h"

#include <QRect>
#include <QString>

#include <memory>
#include <vector>

class TomoGeometry
{
public:
    TomoGeometry( const QString & p_xmlGeometryFilePath );
    ~TomoGeometry() = default;

    bool IsValid() const { return m_isValid; }

    // accessors
    QString filePath() const { return m_filePath; }

    Position3D rotationCenter() const;

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

    // mainly used for parallelization
    std::vector<int> volumeVoxelsIndices() const;

    TomoVolume * GetVolume() const { return m_volume.get(); }
    TomoProjectionsSet * GetProjections() const { return m_projections.get(); }
    TomoProjectionsSet * GetProjectionsRois() const { return m_projectionsRois.get(); }
    TomoTable * GetTable() const { return m_table.get(); }

    Pixel GetProjectionROIsBLPixelPositionOnDetector() const { return m_projectionROIsBLPixelPositionOnDetector; }

private:
    QString m_filePath;    // OK

    // volume geometric features
    std::unique_ptr<TomoVolume> m_volume;
    std::unique_ptr<TomoProjectionsSet> m_projections;
    std::unique_ptr<TomoProjectionsSet> m_projectionsRois;
    std::unique_ptr<TomoTable> m_table;
    Pixel m_projectionROIsBLPixelPositionOnDetector;

    bool m_isValid{ false };

    // parsing utils
    Position3D QStringToPosition3D( const QString & p_positionString, bool & p_done ) const;
    Size3D QStringToSize3D( const QString & p_positionString, bool & p_done ) const;
    QRect QStringToQRect( const QString & p_positionString, bool & p_done ) const;
};

inline bool operator==( TomoGeometry const & p_tomoGeometryA, TomoGeometry const & p_tomoGeometryB )
{
    return p_tomoGeometryA.filePath() == p_tomoGeometryB.filePath();
}

inline bool operator!=( TomoGeometry const & p_tomoGeometryA, TomoGeometry const & p_tomoGeometryB )
{
    return !( p_tomoGeometryA == p_tomoGeometryB );
}
//inline QDebug operator<<( QDebug p_debug, const TomoGeometry & p_tomoGeometry )
//{
//    QDebugStateSaver saver( p_debug );
//    p_debug.nospace() << "___________________GEOMETRY INFORMATION (START)__________________" << Qt::endl;
//
//
//    p_debug.nospace() << "Rotation Center In World: (" << p_tomoGeometry.rotationCenterInWorld[0] << " , " << p_tomoGeometry.rotationCenterInWorld[1]
//                      << " , " << p_tomoGeometry.rotationCenterInWorld[2] << ")" << Qt::endl
//                      << Qt::endl;
//
//
//    p_debug.nospace() << "Detector Dimensions: (" << p_tomoGeometry.projectionsSizeInPixels[0] << ","
//                      << p_tomoGeometry.projectionsSizeInPixels[1] << ")" << Qt::endl;
//    p_debug.nospace() << "Detector Pixel Spacing: (" << p_tomoGeometry.projectionPixelSpacing[0] << ","
//                      << p_tomoGeometry.projectionPixelSpacing[1] << ")" << Qt::endl
//                      << Qt::endl;
//
//    if( p_tomoGeometry.xRaySourcesPositionsInWorld.size() == p_tomoGeometry.detectorCenterPositionsInWorld.size() )
//    {
//        for( unsigned projectionIndex{ 0 }; projectionIndex < p_tomoGeometry.xRaySourcesPositionsInWorld.size(); ++projectionIndex )
//        {
//            p_debug.nospace() << "Projection " << projectionIndex << ":" << Qt::endl;
//            p_debug.nospace() << "Source Position In World: (" << p_tomoGeometry.xRaySourcesPositionsInWorld[projectionIndex][0] << ","
//                              << p_tomoGeometry.xRaySourcesPositionsInWorld[projectionIndex][1] << ","
//                              << p_tomoGeometry.xRaySourcesPositionsInWorld[projectionIndex][2] << ")" << Qt::endl;
//            p_debug.nospace() << "Detector Center Position In World: (" << p_tomoGeometry.detectorCenterPositionsInWorld[projectionIndex][0] << ","
//                              << p_tomoGeometry.detectorCenterPositionsInWorld[projectionIndex][1] << ","
//                              << p_tomoGeometry.detectorCenterPositionsInWorld[projectionIndex][2] << ")" << Qt::endl;
//        }
//        p_debug.nospace() << Qt::endl;
//    }
//
//
//    p_debug.nospace() << "Roi vertices: (" << p_tomoGeometry.projectionsRoi[0][0] << ","
//                      << p_tomoGeometry.projectionsRoi[0][1] << ")   (" << p_tomoGeometry.projectionsRoi[1][0] << ","
//                      << p_tomoGeometry.projectionsRoi[1][1] << ")   (" << p_tomoGeometry.projectionsRoi[2][0] << ","
//                      << p_tomoGeometry.projectionsRoi[2][1] << ")   (" << p_tomoGeometry.projectionsRoi[3][0] << ","
//                      << p_tomoGeometry.projectionsRoi[3][1] << ")   (" << Qt::endl;
//    p_debug.nospace() << "Reconstruction Volume Center Position In World: (" << p_tomoGeometry.reconstructionVolumeCenterPositionInWorld[0]
//                      << " , " << p_tomoGeometry.reconstructionVolumeCenterPositionInWorld[1] << " , "
//                      << p_tomoGeometry.reconstructionVolumeCenterPositionInWorld[2] << ")" << Qt::endl;
//    p_debug.nospace() << "Reconstruction Volume Dimensions: (" << p_tomoGeometry.reconstructionVolumeSizeInPixels[0]
//                      << " , " << p_tomoGeometry.reconstructionVolumeSizeInPixels[1] << " , "
//                      << p_tomoGeometry.reconstructionVolumeSizeInPixels[2] << ")" << Qt::endl;
//    p_debug.nospace() << "Reconstruction Volume Pixel Spacing: (" << p_tomoGeometry.reconstructionVolumePixelSpacing[0]
//                      << " , " << p_tomoGeometry.reconstructionVolumePixelSpacing[1] << " , "
//                      << p_tomoGeometry.reconstructionVolumePixelSpacing[2] << ")" << Qt::endl;
//
//    p_debug.nospace() << "___________________GEOMETRY INFORMATION (END)____________________" << Qt::endl;
//    return p_debug;
//}
