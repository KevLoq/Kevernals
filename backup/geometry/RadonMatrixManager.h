#pragma once


#include "commons/RadonMatrices.h"
#include "commons/Result.h"
#include "modules/geometry/TomoGeometry.h"

#include <vector>

class RadonMatrixManager
{
public:
    struct RadonMatrixComputationOptimizationElements
    {
        RadonMatrixComputationOptimizationElements( TomoGeometry * p_tomoGeometry )
          : projectionRoisIndiceAndPixels( p_tomoGeometry->projectionRoisIndiceAndPixelsPtr() )
          , projectionsRoisPositions( p_tomoGeometry->projectionsRoisPositionsPtr() )
          , detectorsZCommonPosition( p_tomoGeometry->detectorsZCommonPosition() )
          , sourcesXCommonPosition( p_tomoGeometry->sourcesXCommonPosition() )
          , sourcesYPositions( p_tomoGeometry->sourcesYPositionsPtr() )
          , sourcesZCommonPosition( p_tomoGeometry->sourcesZCommonPosition() )
          , volumeSize3D( p_tomoGeometry->volumeSize3D() )
          , volumeZs( p_tomoGeometry->volumeZsPtr() )
          , volumeYs( p_tomoGeometry->volumeYsPtr() )
          , volumeXs( p_tomoGeometry->volumeXsPtr() )
          , volumeBLF( p_tomoGeometry->volumeBLF() )
          , volumeTRB( p_tomoGeometry->volumeTRB() )
          , volumeVoxelSpacing( p_tomoGeometry->volumeVoxelSpacing() )
        {}

        const std::vector<PixelOnProjection> * projectionRoisIndiceAndPixels;    // m_tomoGeometry->projectionRoisIndiceAndPixels()
        const std::vector<Position2D> * projectionsRoisPositions;                // m_tomoGeometry->projectionsRoisPositions()
        float detectorsZCommonPosition;                                          // m_tomoGeometry->detectorsZCommonPosition()
        float sourcesXCommonPosition;                                            // m_tomoGeometry->sourcesXCommonPosition()
        const std::vector<float> * sourcesYPositions;                            // m_tomoGeometry->sourcesYPositions()
        float sourcesZCommonPosition;                                            // m_tomoGeometry->sourcesZCommonPosition()
        Size3D volumeSize3D;                                                     // m_tomoGeometry->volumeSize3D()
        const std::vector<float> * volumeZs;                                     // m_tomoGeometry->volumeZs()
        const std::vector<float> * volumeYs;                                     // m_tomoGeometry->volumeYs()
        const std::vector<float> * volumeXs;                                     // m_tomoGeometry->volumeXs()
        Position3D volumeBLF;                                                    // m_tomoGeometry->volumeBLF()
        Position3D volumeTRB;                                                    // m_tomoGeometry->volumeTRB()
        VoxelSpacing volumeVoxelSpacing;                                         // m_tomoGeometry->volumeVoxelSpacing()
    };

    enum class RadonMatrixComputationMethod
    {
        RayTracing,
        Binary
    };

    RadonMatrixManager( TomoGeometry * p_tomoGeometry )
      : m_tomoGeometry{ p_tomoGeometry } {};
    ~RadonMatrixManager() = default;
    Result<RadonMatrix> LoadRadonMatrix( const QString & p_radonMatrixFilePath );
    Result<void> SaveRadonMatrix( const RadonMatrix & p_radonMatrix, const QString & p_radonMatrixFilePath );

    Result<std::unique_ptr<RadonMatrix>> ComputeProjectionMatrix( RadonMatrixComputationMethod p_method = RadonMatrixComputationMethod::Binary );
    // see matrix computation method
    Result<std::unique_ptr<RadonMatrix>> ComputeItsBackProjectionMatrix( RadonMatrix * p_projectionMatrix );

private:
    Result<std::unique_ptr<RadonMatrix>> ComputeProjectionMatrixByBinaryMethod();
    Result<std::unique_ptr<RadonMatrix>> ComputeProjectionMatrixByRayTracingMethod();
    TomoGeometry * m_tomoGeometry;
};
