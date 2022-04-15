#include "modules/projection/RayTracingProjector.h"

#include "commons/GeometryUtils.h"
#include "commons/Maths.h"
#include "modules/geometry/RadonMatrixManager.h"
#include "modules/projection/ProjectorErrorCode.h"

#include <QDebug>
#include <vtkImageCast.h>

#include <algorithm>
#include <execution>
#include <numeric>

Result<void> RayTracingProjector::SetGeometry( const TomoGeometry & p_tomoGeometry )
{
    if( m_tomoGeometry != p_tomoGeometry )
    {
        m_tomoGeometry = p_tomoGeometry;
        auto translationsComputationResult = this->ComputeRadonMatrices();
        if( translationsComputationResult.has_error() )
        {
            return translationsComputationResult.error();
        }
    }
    return outcome::success();
}

Result<ImageDataPtr> RayTracingProjector::PerformProjection( ImageDataPtr p_volume )
{
    /* if( p_volume == nullptr )
    {
        auto errorCode = TomosynthesisReconstructorErrorCode::VolumeProblemForProjection;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    if( p_projectionIndex < 0 || p_projectionIndex >= m_tomoGeometry.detectorCenterPositionsInWorld.size() )
    {
        auto errorCode = TomosynthesisReconstructorErrorCode::RadonMatrixComputation;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    auto volumeDimensions = p_volume->GetDimensions();
    auto geometryVolumeSize = m_tomoGeometry.reconstructionVolumeSizeInPixels;

    if( volumeDimensions[0] != geometryVolumeSize[0] || volumeDimensions[1] != geometryVolumeSize[1] || volumeDimensions[2] != geometryVolumeSize[2] )
    {
        auto errorCode = TomosynthesisReconstructorErrorCode::VolumeProblemForProjection;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    auto currentRoi = m_tomoGeometry.projectionRois[p_projectionIndex];
    auto minXRoi = m_tomoGeometry.projectionsSizeInPixels[0];
    auto maxXRoi = 0;
    auto minYRoi = m_tomoGeometry.projectionsSizeInPixels[1];
    auto maxYRoi = 0;
    for( const auto & vertex : currentRoi )
    {
        if( vertex[0] < minXRoi )
        {
            minXRoi = vertex[0];
        }
        if( vertex[0] > maxXRoi )
        {
            maxXRoi = vertex[0];
        }

        if( vertex[1] < minYRoi )
        {
            minYRoi = vertex[1];
        }
        if( vertex[1] > maxYRoi )
        {
            maxYRoi = vertex[1];
        }
    }
    if( minXRoi >= maxXRoi || minYRoi >= maxYRoi )
    {
        auto errorCode = TomosynthesisReconstructorErrorCode::RadonMatrixComputation;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    auto projectionImage = ImageDataPtr::New();
    auto currentIndex{ 0 };
    projectionImage->SetDimensions( m_tomoGeometry.projectionsSizeInPixels[0], m_tomoGeometry.projectionsSizeInPixels[1], 1 );
    projectionImage->AllocateScalars( VTK_FLOAT, 1 );
    auto projectionDimension = projectionImage->GetDimensions();
    auto radonMatrix = m_radonProjectionMatrix[p_projectionIndex];
    auto dimX = maxXRoi - minXRoi;
    auto projectionPixelIndex{ 0 };
    for( const auto & projectionRay : radonMatrix )
    {
        auto projectionPixelX = minXRoi + projectionPixelIndex % dimX;
        auto projectionPixelY = minYRoi + projectionPixelIndex / dimX;
        auto pixel = static_cast<float *>( projectionImage->GetScalarPointer( projectionPixelX, projectionPixelY, 0 ) );
        pixel[0] = 0.f;
        for( const auto & involvedVoxel : projectionRay )
        {
            auto voxel = static_cast<unsigned short *>( p_volume->GetScalarPointer( involvedVoxel.first[0], involvedVoxel.first[1], involvedVoxel.first[2] ) );
            pixel[0] += static_cast<float>( involvedVoxel.second ) * static_cast<float>( voxel[0] );
        }
        projectionPixelIndex++;
    }

    auto rescaler = vtkSmartPointer<vtkImageCast>::New();
    rescaler->SetInputData( projectionImage );
    rescaler->SetOutputScalarTypeToUnsignedShort();
    rescaler->Update();

    return rescaler->GetOutput();
    */
    return nullptr;
}

Result<ImageDataPtr> RayTracingProjector::PerformBackprojection( ImageDataPtr p_projection )
{
    return nullptr;
}

//Result<void> RayTracingProjector::ComputeRadonMatrices( std::optional<QString> p_radonProjectionMatrixFilePath, bool p_saveRadonMatrix )
//{
//    if( p_radonProjectionMatrixFilePath.has_value() )
//    {
//        auto radonLoadingResult = RadonMatrixManager::LoadRadonProjectionMatrix( p_radonProjectionMatrixFilePath.value() );
//        if( radonLoadingResult.has_error() )
//        {
//            auto radonComputationResult = RadonMatrixManager::ComputeRadonProjectionMatrix( m_tomoGeometry );
//            if( radonComputationResult.has_error() )
//            {
//                return radonComputationResult.error();
//            }
//            m_radonProjectionMatrix = radonComputationResult.value();
//            if( p_saveRadonMatrix )
//            {
//                auto radonSavingResult = RadonMatrixManager::SaveRadonProjectionMatrix( m_radonProjectionMatrix, p_radonProjectionMatrixFilePath.value() );
//                if( radonSavingResult.has_error() )
//                {
//                    return radonSavingResult.error();
//                }
//            }
//            return outcome::success();
//        }
//        m_radonProjectionMatrix = radonLoadingResult.value();
//        return outcome::success();
//    }
//
//    auto radonComputationResult = RadonMatrixManager::ComputeRadonProjectionMatrix( m_tomoGeometry );
//    if( radonComputationResult.has_error() )
//    {
//        return radonComputationResult.error();
//    }
//    m_radonProjectionMatrix = radonComputationResult.value();
//    return outcome::success();
//}


Result<void> RayTracingProjector::ComputeRadonMatrices()
{
    auto radonProjectionComputationResult = this->ComputeRadonProjectionMatrix();
    if( radonProjectionComputationResult.has_error() )
    {
        return radonProjectionComputationResult.error();
    }
    auto radonBackProjectionComputationResult = this->ComputeRadonBackProjectionMatrix();
    if( radonBackProjectionComputationResult.has_error() )
    {
        return radonBackProjectionComputationResult.error();
    }
    return outcome::success();
}

//TODO
Result<void> RayTracingProjector::ComputeRadonBackProjectionMatrix()
{
    return outcome::success();
}

Result<void> RayTracingProjector::ComputeRadonProjectionMatrix()
{
    return outcome::success();
}
