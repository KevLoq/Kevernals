#include "modules/geometry/RadonMatrixManager.h"

#include "modules/geometry/RadonMatrixErrorCode.h"

#include <QDataStream>
#include <QDebug>
#include <QFile>

#include <algorithm>
#include <execution>
#include <memory>
#include <mutex>
#include <numeric>

Result<RadonMatrix> RadonMatrixManager::LoadRadonMatrix( const QString & p_radonMatrixFilePath )
{
    qDebug() << " RadonMatrixManager::LoadRadonProjectionMatrix";
    QFile radonProjectionFile{ p_radonMatrixFilePath };
    if( !radonProjectionFile.exists() )
    {
        auto errorCode = RadonMatrixErrorCode::FileLoadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    if( !radonProjectionFile.open( QIODevice::ReadOnly ) )
    {
        auto errorCode = RadonMatrixErrorCode::FileLoadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    QDataStream in( &radonProjectionFile );
    RadonMatrix radonMatrix;
    int nbProjections;
    in >> nbProjections;
    /*
    radonProjectionMatrix.resize( nbProjections );
    for( auto projectionIndex{ 0 }; projectionIndex < nbProjections; ++projectionIndex )
    {
        int nbProjectionPixels;
        in >> nbProjectionPixels;
        radonProjectionMatrix[projectionIndex].resize( nbProjectionPixels );
        for( auto projectionPixelIndex{ 0 }; projectionPixelIndex < nbProjectionPixels; ++projectionPixelIndex )
        {
            int nbInvolvedVoxels;
            in >> nbInvolvedVoxels;
            radonProjectionMatrix[projectionIndex][projectionPixelIndex].resize( nbInvolvedVoxels );

            for( auto involvedVoxelIndex{ 0 }; involvedVoxelIndex < nbInvolvedVoxels; ++involvedVoxelIndex )
            {
                auto & involvedVoxel = radonProjectionMatrix[projectionIndex][projectionPixelIndex][involvedVoxelIndex];
                in >> involvedVoxel.first.x;
                in >> involvedVoxel.first.y;
                in >> involvedVoxel.first.z;
                in >> involvedVoxel.second;
            }
        }
    }*/
    return radonMatrix;
}

Result<void> RadonMatrixManager::SaveRadonMatrix( const RadonMatrix & p_radonMatrix, const QString & p_radonMatrixFilePath )
{
    (void)p_radonMatrix;
    qDebug() << "RadonMatrixManager::LoadRadonProjectionMatrix";

    QFile radonProjectionFile{ p_radonMatrixFilePath };
    if( !radonProjectionFile.open( QIODevice::WriteOnly ) )
    {
        auto errorCode = RadonMatrixErrorCode::FileSavingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    /* QDataStream out( &radonProjectionFile );
    auto nbProjections = p_radonMatrix.size();
    out << nbProjections;
    for( auto projectionIndex{ 0 }; projectionIndex < nbProjections; ++projectionIndex )
    {
        auto nbProjectionPixels = p_radonMatrix[projectionIndex].size();
        out << nbProjectionPixels;
        for( auto projectionPixelIndex{ 0 }; projectionPixelIndex < nbProjectionPixels; ++projectionPixelIndex )
        {
            auto nbInvolvedVoxels = p_radonMatrix[projectionIndex][projectionPixelIndex].size();
            out << nbInvolvedVoxels;

            for( auto involvedVoxelIndex{ 0 }; involvedVoxelIndex < nbInvolvedVoxels; ++involvedVoxelIndex )
            {
                auto involvedVoxel = p_radonMatrix[projectionIndex][projectionPixelIndex][involvedVoxelIndex];
                out << involvedVoxel.first.x;
                out << involvedVoxel.first.y;
                out << involvedVoxel.first.z;
                out << involvedVoxel.second;
            }
        }
    }*/
    return outcome::success();
}


Result<std::unique_ptr<RadonMatrix>> RadonMatrixManager::ComputeProjectionMatrix( RadonMatrixComputationMethod p_method )
{
    if( p_method == RadonMatrixComputationMethod::RayTracing )
    {
        return ComputeProjectionMatrixByRayTracingMethod();
    }
    else    // binary is default method
    {
        return ComputeProjectionMatrixByBinaryMethod();
    }
}

Result<std::unique_ptr<RadonMatrix>> RadonMatrixManager::ComputeProjectionMatrixByBinaryMethod()
{
    qDebug() << "RadonMatrixManager::ComputeProjectionMatrixByBinaryMethod";
    return std::make_unique<RadonMatrix>();
}
Result<std::unique_ptr<RadonMatrix>> RadonMatrixManager::ComputeItsBackProjectionMatrix( RadonMatrix * p_projectionMatrix )
{
    auto backProjectionMatrix = std::make_unique<RadonMatrix>();
    //auto volumeSize3D = m_tomoGeometry->volumeSize3D();
    //auto completeDataSize = volumeSize3D.x * volumeSize3D.y * volumeSize3D.z;
    //backProjectionMatrix->agregationPositions.resize( completeDataSize );
    //backProjectionMatrix->agregationPositions.resize( completeDataSize );
    //auto projectionMatrixSize = static_cast<int>( p_projectionMatrix->size() );
    //for( auto projectionIndex = 0; projectionIndex < projectionMatrixSize; projectionIndex++ )
    //{
    //    auto nbAggregates = p_projectionMatrix->at( projectionIndex ).size();

    //    for( size_t aggregateIndex = 0; aggregateIndex < nbAggregates; aggregateIndex++ )
    //    {
    //        RadonMatrixElement currentElement;
    //        currentElement.position = projectionIndex;
    //        //auto involvedVoxelGlobalIndex = p_projectionMatrix->at( projectionIndex ).at( aggregateIndex ).position;
    //        currentElement.weight = p_projectionMatrix->at( projectionIndex ).at( aggregateIndex ).weight;
    //        ( *backProjectionMatrix )[p_projectionMatrix->at( projectionIndex ).at( aggregateIndex ).position].push_back( currentElement );
    //    }
    //}
    //// renormalization

    //for( auto voxelIndex = 0; voxelIndex < completeDataSize; voxelIndex++ )
    //{
    //    auto nbAggregates = backProjectionMatrix->at( voxelIndex ).size();
    //    auto totalWeight = 0.f;
    //    for( size_t aggregateIndex = 0; aggregateIndex < nbAggregates; aggregateIndex++ )
    //    {
    //        //auto involvedVoxelGlobalIndex = p_projectionMatrix->at( projectionIndex ).at( aggregateIndex ).position;
    //        totalWeight += backProjectionMatrix->at( voxelIndex ).at( aggregateIndex ).weight;
    //    }
    //    if( totalWeight > 0 )
    //    {
    //        auto weightFactor = 1. / totalWeight;
    //        for( size_t aggregateIndex = 0; aggregateIndex < nbAggregates; aggregateIndex++ )
    //        {
    //            ( *backProjectionMatrix )[voxelIndex][aggregateIndex].weight *= weightFactor;
    //        }
    //    }
    //}


    return backProjectionMatrix;
}

Result<std::unique_ptr<RadonMatrix>> RadonMatrixManager::ComputeProjectionMatrixByRayTracingMethod()
{
    qDebug() << "RadonMatrixManager::ComputeProjectionMatrixByRayTracingMethod";

    auto projectionIndices = m_tomoGeometry->projectionsRoisPositionsIndicesPtr();

    //RadonMatrixElement currentElement;
    //qDebug() << "sizeof( RadonMatrixElement ) = " << sizeof( RadonMatrixElement );
    //qDebug() << "sizeof( std::vector<RadonMatrixElement> ) = " << sizeof( std::vector<RadonMatrixElement> );
    //qDebug() << "sizeof( RadonMatrixAggregate ) = " << sizeof( RadonMatrixAggregate );
    //qDebug() << "sizeof( std::vector<RadonMatrixAggregate> ) = " << sizeof( std::vector<RadonMatrixAggregate> );
    auto projectionMatrix = std::make_unique<RadonMatrix>();
    projectionMatrix->agregationPositions.resize( projectionIndices->size() );
    projectionMatrix->agregationSizes.resize( projectionIndices->size() );


    qDebug() << "sizeof( projectionMatrix ) = " << sizeof( projectionMatrix );

    //std::mutex computationMutex;
    size_t totalSize{ 0 };
    RadonMatrixComputationOptimizationElements optimizationElements( m_tomoGeometry );

    auto projectionLineProcessing = [this, &optimizationElements, &projectionMatrix, &totalSize /*, &computationMutex*/]( int p_globalProjectionIndex ) {
        // general conversion rule :
        //   - index to coordinates              z = index / sizeX*sizeY ;
        //                                       y = (index / sizeX) % sizeY ;  Due to rewrite :  index = ( z * sizeY + y ) * sizeX + x
        //                                       x = index % sizeX ;  Due to rewrite :  index = ( z * sizeY + y ) * sizeX + x

        //if( p_globalProjectionIndex == 4084923 )
        //{
        //    qInfo() << "HERE";
        //}
        auto pixelOnProjection = ( optimizationElements.projectionRoisIndiceAndPixels )->at( p_globalProjectionIndex );
        auto projectionPosition2D = optimizationElements.projectionsRoisPositions->at( p_globalProjectionIndex );
        auto projectionPosition3D = Position3D( projectionPosition2D.x, projectionPosition2D.y, optimizationElements.detectorsZCommonPosition );

        auto projectionIndex = pixelOnProjection.projectionIndex;
        auto sourcePosition = Position3D( optimizationElements.sourcesXCommonPosition, optimizationElements.sourcesYPositions->at( projectionIndex ), optimizationElements.sourcesZCommonPosition );

        // qInfo() << "global projection index : " << p_globalProjectionIndex;

        Vector3D rayDirectorVector = projectionPosition3D - sourcePosition;

        //  Following condition should never happen in tomosynthesis
        // But if one day ;) we would have tomo system with robotics arms, it could be usefull to chock horizonality
        if( AlmostEqualRelative( rayDirectorVector.z, 0.f ) )
        {
            qCritical() << "rayDirectorVector should not have a null z component";
            return;
        }
        auto intersectionPoints = m_tomoGeometry->GetVolume()->FindIntersectionPointsWithLine( sourcePosition, rayDirectorVector );
        if( intersectionPoints.empty() )
        {
            return;
        }

        auto isDirectOrientationInY = rayDirectorVector.y > 0 ? 1 : -1;

        // improve the following check
        // sort by increasing x (arbitrary)
        std::sort( intersectionPoints.begin(), intersectionPoints.end(), []( Position3D a, Position3D b ) {
            return a.z > b.z;
        } );

        auto totalLength = intersectionPoints.front().DistanceTo( intersectionPoints.back() );
        auto weightFactor = 1.f / totalLength;
        //check the middle points voxel belonging

        const auto nbIntersectionPoints = static_cast<int>( intersectionPoints.size() );
        const auto avgDistance = totalLength / static_cast<float>( nbIntersectionPoints - 1 );
        projectionMatrix->agregationPositions[p_globalProjectionIndex] = projectionMatrix->positions.size();
        uint16_t raySize{ 0 };
        for( auto intersectionPointsIndex{ 0 }; intersectionPointsIndex < intersectionPoints.size() - 1; ++intersectionPointsIndex )
        {
            Position3D center;
            center.x = 0.5 * ( intersectionPoints[intersectionPointsIndex].x + intersectionPoints[intersectionPointsIndex + 1].x );
            center.y = 0.5 * ( intersectionPoints[intersectionPointsIndex].y + intersectionPoints[intersectionPointsIndex + 1].y );
            center.z = 0.5 * ( intersectionPoints[intersectionPointsIndex].z + intersectionPoints[intersectionPointsIndex + 1].z );

            auto involvedVoxel = m_tomoGeometry->GetVolume()->FindVoxelContainingPosition( center );

            if( involvedVoxel.x < 0 || involvedVoxel.x >= optimizationElements.volumeSize3D.x
                || involvedVoxel.y < 0 || involvedVoxel.y >= optimizationElements.volumeSize3D.y
                || involvedVoxel.z < 0 || involvedVoxel.z >= optimizationElements.volumeSize3D.z )
            {
                qCritical() << "involved voxel outside volume: (" << involvedVoxel.x << "," << involvedVoxel.y << "," << involvedVoxel.z << ")";
            }
            else
            {
                auto diffX = ( intersectionPoints[intersectionPointsIndex].x - intersectionPoints[intersectionPointsIndex + 1].x );
                auto diffY = ( intersectionPoints[intersectionPointsIndex].y - intersectionPoints[intersectionPointsIndex + 1].y );
                auto diffZ = ( intersectionPoints[intersectionPointsIndex].z - intersectionPoints[intersectionPointsIndex + 1].z );
                auto distance = std::sqrt( diffX * diffX + diffY * diffY + diffZ * diffZ );

                if( distance > 0.2 * avgDistance )
                {
                    projectionMatrix->positions.push_back( involvedVoxel.z * optimizationElements.volumeSize3D.x * optimizationElements.volumeSize3D.y
                                                           + involvedVoxel.y * optimizationElements.volumeSize3D.x
                                                           + involvedVoxel.x );
                    //projectionMatrix->positionsAndWeights.push_back( positionAndWeight );
                    raySize++;
                    totalSize++;
                }
            }
        }

        projectionMatrix->agregationSizes[p_globalProjectionIndex] = raySize;
    };

    ////4084923
    //std::vector<int> reducedIndices;
    //for( auto iterator{ 0 }; iterator < 40000; iterator++ )
    //{
    //    reducedIndices.push_back( projectionIndices->at( iterator ) );
    //}
    std::for_each( std::execution::seq, projectionIndices->cbegin(), projectionIndices->cend(), projectionLineProcessing );
    //std::for_each( std::execution::par_unseq, reducedIndices.cbegin(), reducedIndices.cend(), projectionLineProcessing );

    qDebug() << "totalSize = " << totalSize;
    qDebug() << "sizeof( float ) = " << sizeof( float );
    qDebug() << "sizeof( int ) = " << sizeof( int );
    qDebug() << "sizeof(*projectionMatrix) = " << sizeof( *projectionMatrix );
    qDebug() << "sizeof(*projectionMatrix) = " << totalSize * ( sizeof( int ) + sizeof( char ) );

    return projectionMatrix;
}