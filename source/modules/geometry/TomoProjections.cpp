#pragma once

#include "modules/geometry/TomoGeometry.h"

#include "commons/GeometryUtils.h"

#include <QDomDocument>
#include <QFile>
#include <QPoint>


/*
 const QString filePath;

    const PositionIn3DWorld rotationCenterInWorld{ 0., 0., 0. };

    const QVector<float> sourcesYPositionsInWorld;
    const float sourcesXPositionInWorld{ 0.f };
    const float sourcesZPositionInWorld{ 0.f };

    const float sid{ 0.f };
    const QVector<float> detectorCenterYPositionsInWorld;
    const float detectorCentersXPositionInWorld{ 0.f };
    const float detectorCentersZPositionInWorld{ 0.f };

    const QVector<PositionIn2DWorld> detectorsBLPositionsInWorld;
    const Size2D detectorsSizeInPixels{ 0, 0 };
    const PixelSpacing2D detectorsPixelSpacing{ 0., 0. };
const Size2DInWorld detectorsSizeInWorld{ 0., 0. };

    const PositionIn3DWorld volumeCenterPositionInWorld{ 0., 0., 0. };
    const Size3D volumeSizeInPixels{ 0, 0, 0 };
    const PixelSpacing3D volumePixelSpacing{ 0., 0., 0. };
    const Size3DInWorld volumeSizeInWorld{ 0., 0., 0. };
    const PositionIn3DWorld volumeBLD{ 0., 0., 0. };
    const PositionIn3DWorld volumeTRF{ 0., 0., 0. };

    const QVector<float> volumeZs;
    const QVector<float> volumeYs;
    const QVector<float> volumeXs;

    const int nbProjections{ 0 };
    const QVector<QVector<float>> projectionsXPositionsInWorld;
    const QVector<QVector<float>> projectionsYPositionsInWorld;


    // mainly used for parallelization
    const QVector<int> projectionIndices;
    // mainly used for parallelization
    const QVector<int> volumeIndices;
*/
TomoGeometry::TomoGeometry( const QString & p_xmlGeometryFilePath, bool & p_successfullConstruction )
{
    /***************************
    xml file check
    */
    QFile xmlGeometryFile{ p_xmlGeometryFilePath };
    QDomDocument xmlDoc( "manifest" );
    if( !xmlGeometryFile.exists() && !xmlGeometryFile.open( QIODevice::ReadOnly ) )
    {
        p_successfullConstruction = false;
        return;
    }
    if( !xmlDoc.setContent( &xmlGeometryFile ) )
    {
        p_successfullConstruction = false;
        return;
    }
    xmlGeometryFile.close();

    auto geometryElem = xmlDoc.documentElement();
    if( geometryElem.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }


    m_filePath = p_xmlGeometryFilePath;
    /***************************
    Fulcrum position
    */
    auto rotationXmlElement = geometryElem.firstChildElement( "Rotation" );
    if( rotationXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }

    auto rotationCenterXmlElement = rotationXmlElement.firstChildElement( "center" );
    if( rotationCenterXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }


    auto positionString = rotationCenterXmlElement.firstChild().nodeValue();
    bool successfullParse;
    m_rotationCenterInWorld = QStringToPositionIn3DWorld( positionString, successfullParse );
    if( !successfullParse )
    {
        p_successfullConstruction = false;
        return;
    }


    /***************************
    XRay sources positions
    */
    auto xRayXmlElement = geometryElem.firstChildElement( "XRay" );
    if( xRayXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto sourcesXmlElement = xRayXmlElement.firstChildElement( "sources" );
    if( sourcesXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    // Get the first child of the component
    auto childSource = sourcesXmlElement.firstChild().toElement();
    QVector<float> xSourcePositions;
    QVector<float> zSourcePositions;
    // Read each child of the sources node
    while( !childSource.isNull() )
    {
        auto sourcePositionString = childSource.firstChild().nodeValue();
        auto sourcePositionInWorld = QStringToPositionIn3DWorld( sourcePositionString, successfullParse );
        if( !successfullParse )
        {
            p_successfullConstruction = false;
            return;
        }
        xSourcePositions.push_back( sourcePositionInWorld.x );
        m_sourcesYPositionsInWorld.push_back( sourcePositionInWorld.y );
        zSourcePositions.push_back( sourcePositionInWorld.z );
        // Next child
        childSource = childSource.nextSibling().toElement();
    }
    if( m_sourcesYPositionsInWorld.empty() )
    {
        p_successfullConstruction = false;
        return;
    }

    // Y source positions are filled
    // X and Z are taken as average value
    m_sourcesXPositionInWorld = static_cast<float>( std::accumulate( xSourcePositions.begin(), xSourcePositions.end(), .0 ) ) / static_cast<float>( xSourcePositions.size() );
    m_sourcesZPositionInWorld = static_cast<float>( std::accumulate( zSourcePositions.begin(), zSourcePositions.end(), .0 ) ) / static_cast<float>( zSourcePositions.size() );

    /***************************
    Detectors positions and sizes
    */
    auto cameraXmlElement = geometryElem.firstChildElement( "Camera" );
    if( cameraXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto totalWidthXmlElement = cameraXmlElement.firstChildElement( "totalWidth" );
    if( totalWidthXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto conversionOK{ true };
    auto cameraWidth = totalWidthXmlElement.firstChild().nodeValue().toFloat( &conversionOK );
    if( !conversionOK || AlmostEqualRelative( cameraWidth, 0.f ) )
    {
        p_successfullConstruction = false;
        return;
    }
    m_detectorsSizeInWorld.x = cameraWidth;
    auto totalHeightXmlElement = cameraXmlElement.firstChildElement( "totalHeight" );
    if( totalHeightXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto cameraHeight = totalHeightXmlElement.firstChild().nodeValue().toFloat( &conversionOK );
    if( !conversionOK || AlmostEqualRelative( cameraHeight, 0.f ) )
    {
        p_successfullConstruction = false;
        return;
    }
    m_detectorsSizeInWorld.y = cameraHeight;
    auto pixelWidthXmlElement = cameraXmlElement.firstChildElement( "pixelWidth" );
    if( pixelWidthXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto pixelWidth = pixelWidthXmlElement.firstChild().nodeValue().toInt( &conversionOK );
    if( !conversionOK || pixelWidth == 0 )
    {
        p_successfullConstruction = false;
        return;
    }
    m_detectorsSizeInPixels.x = pixelWidth;
    auto pixelHeightXmlElement = cameraXmlElement.firstChildElement( "pixelHeight" );
    if( pixelHeightXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto pixelHeight = pixelHeightXmlElement.firstChild().nodeValue().toInt( &conversionOK );
    if( !conversionOK || pixelHeight == 0 )
    {
        p_successfullConstruction = false;
        return;
    }
    m_detectorsSizeInPixels.y = pixelHeight;

    m_detectorsPixelSpacing.x = m_detectorsSizeInWorld.x / static_cast<double>( m_detectorsSizeInPixels.x );
    m_detectorsPixelSpacing.y = m_detectorsSizeInWorld.y / static_cast<double>( m_detectorsSizeInPixels.y );

    auto referencesXmlElement = cameraXmlElement.firstChildElement( "references" );
    if( referencesXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    // Get the first child of the component
    auto childReference = referencesXmlElement.firstChild().toElement();
    QVector<float> xDetectorBLPositions;
    QVector<float> xDetectorCenterPositions;
    QVector<float> zDetectorPositions;
    // Read each child of the sources node
    while( !childReference.isNull() )
    {
        auto detectorPositionString = childReference.firstChild().nodeValue();
        auto detectorPositionInWorld = QStringToPositionIn3DWorld( detectorPositionString, successfullParse );
        if( !successfullParse )
        {
            p_successfullConstruction = false;
            return;
        }
        xDetectorBLPositions.push_back( detectorPositionInWorld.x );
        m_detectorsBLYPositionsInWorld.push_back( detectorPositionInWorld.y );
        zDetectorPositions.push_back( detectorPositionInWorld.z );

        xDetectorCenterPositions.push_back( detectorPositionInWorld.x + 0.5 * m_detectorsSizeInWorld.x );
        m_detectorsCentersYPositionsInWorld.push_back( detectorPositionInWorld.y + 0.5 * m_detectorsSizeInWorld.y );
        // Next child
        childReference = childReference.nextSibling().toElement();
    }
    if( m_detectorsCentersYPositionsInWorld.empty() )
    {
        p_successfullConstruction = false;
        return;
    }
    m_nbProjections = m_detectorsCentersYPositionsInWorld.size();

    m_detectorsBLXPositionInWorld = static_cast<float>( std::accumulate( xDetectorBLPositions.begin(), xDetectorBLPositions.end(), .0 ) ) / static_cast<float>( xDetectorBLPositions.size() );
    m_detectorsCentersXPositionInWorld = static_cast<float>( std::accumulate( xDetectorCenterPositions.begin(), xDetectorCenterPositions.end(), .0 ) ) / static_cast<float>( xDetectorCenterPositions.size() );
    m_detectorsZPositionInWorld = static_cast<float>( std::accumulate( zDetectorPositions.begin(), zDetectorPositions.end(), .0 ) ) / static_cast<float>( zDetectorPositions.size() );

    m_detectorsXPositionsInWorld.resize( m_detectorsSizeInPixels.x );
    m_detectorsXPositionsInWorld[0] = m_detectorsBLXPositionInWorld;
    for( auto pixelX = 1; pixelX < m_detectorsSizeInPixels.x; pixelX++ )
    {
        m_detectorsXPositionsInWorld[pixelX] = m_detectorsXPositionsInWorld[pixelX - 1] + m_detectorsPixelSpacing.x;
    }

    m_detectorsYPositionsInWorld.resize( m_nbProjections );
    for( auto projectionIndex = 0; projectionIndex < m_nbProjections; projectionIndex++ )
    {
        m_detectorsYPositionsInWorld[projectionIndex].resize( m_detectorsSizeInPixels.y );
        m_detectorsYPositionsInWorld[projectionIndex][0] = m_detectorsBLYPositionsInWorld[projectionIndex];
        for( auto pixelY = 1; pixelY < m_detectorsSizeInPixels.y; pixelY++ )
        {
            m_detectorsYPositionsInWorld[projectionIndex][pixelY] = m_detectorsYPositionsInWorld[projectionIndex][pixelY - 1] + m_detectorsPixelSpacing.y;
        }
    }


    /***************************
    Projections ROIs
    */
    auto radiosXmlElement = geometryElem.firstChildElement( "Radios" );
    if( radiosXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }

    auto roisXmlElement = radiosXmlElement.firstChildElement( "rois" );
    if( roisXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    // Get the first child of the component
    auto childRois = roisXmlElement.firstChild().toElement();

    auto roiIndex{ 0 };
    // Read each child of the sources node
    while( !childRois.isNull() )
    {
        auto roiString = childRois.firstChild().nodeValue();
        auto roiResult = QStringToQRect( roiString, successfullParse );
        if( !successfullParse )
        {
            p_successfullConstruction = false;
            return;
        }
        if( roiIndex == 0 )
        {
            projectionsRoi = roiResult.value();
        }
        else
        {
            if( projectionsRoi != roiResult.value() )
            {
                qWarning() << " some Rois of projections are different";
            }
        }
        // Next child
        roiIndex++;
        childRois = childRois.nextSibling().toElement();
    }


    minXProjectionRoi = projectionsSizeInPixels[0];
    maxXProjectionRoi = 0;
    minYProjectionRoi = projectionsSizeInPixels[1];
    maxYProjectionRoi = 0;
    for( const auto & vertex : projectionsRoi )
    {
        if( vertex[0] < minXProjectionRoi )
        {
            minXProjectionRoi = vertex[0];
        }
        if( vertex[0] > maxXProjectionRoi )
        {
            maxXProjectionRoi = vertex[0];
        }

        if( vertex[1] < minYProjectionRoi )
        {
            minYProjectionRoi = vertex[1];
        }
        if( vertex[1] > maxYProjectionRoi )
        {
            maxYProjectionRoi = vertex[1];
        }
    }
    if( minXProjectionRoi >= maxXProjectionRoi || minYProjectionRoi >= maxYProjectionRoi )
    {
        p_successfullConstruction = false;
        return;
    }
    projectionRoiXSize = maxXProjectionRoi + 1 - minXProjectionRoi;
    projectionRoiYSize = maxYProjectionRoi + 1 - minYProjectionRoi;
    roiXPositionsInWorld.resize( nbProjections );
    roiYPositionsInWorld.resize( nbProjections );

    QVector<QVector<float>> roiYPositionsInWorld;
    for( auto projectionIndex = 0; projectionIndex < detectorCenterPositionsInWorld.size(); projectionIndex++ )
    {
        auto blProjectionInWorldX = detectorBLPositionsInWorld[projectionIndex][0];
        roiXPositionsInWorld[projectionIndex].resize( projectionRoiXSize );
        roiXPositionsInWorld[projectionIndex][0] = blProjectionInWorldX + static_cast<float>( minXProjectionRoi ) * projectionPixelSpacing[0];
        for( auto pixelX = 1; pixelX < projectionRoiXSize; pixelX++ )
        {
            roiXPositionsInWorld[projectionIndex][pixelX] = roiXPositionsInWorld[projectionIndex][pixelX - 1] + projectionPixelSpacing[0];
        }

        auto blProjectionInWorldY = detectorBLPositionsInWorld[projectionIndex][1];
        roiYPositionsInWorld[projectionIndex].resize( projectionRoiYSize );
        roiYPositionsInWorld[projectionIndex][0] = blProjectionInWorldY + static_cast<float>( minYProjectionRoi ) * projectionPixelSpacing[1];
        for( auto pixelY = 1; pixelY < projectionRoiYSize; pixelY++ )
        {
            roiYPositionsInWorld[projectionIndex][pixelY] = roiYPositionsInWorld[projectionIndex][pixelY - 1] + projectionPixelSpacing[1];
        }
    }

    auto globalProjectionIndex = 0;
    for( auto projectionIndex = 0; projectionIndex < detectorCenterPositionsInWorld.size(); projectionIndex++ )
    {
        for( auto pixelY = 0; pixelY < projectionRoiYSize; pixelY++ )
        {
            for( auto pixelX = 0; pixelX < projectionRoiXSize; pixelX++ )
            {
                projectionIndices.push_back( globalProjectionIndex );
                globalProjectionIndex++;
            }
        }
    }


    /***************************
    Reconstruction volume
    */
    auto gridXmlElement = geometryElem.firstChildElement( "Grid" );    // Grid represents the reconstruction geometry
    if( gridXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }

    auto scaleXmlElement = gridXmlElement.firstChildElement( "scale" );
    if( scaleXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto reconstrucionPixelSpacingResult = QStringToPositionIn3DWorld( scaleXmlElement.firstChild().nodeValue() );
    if( reconstrucionPixelSpacingResult.has_error() )
    {
        p_successfullConstruction = false;
        return;
    }
    // z an x are inverted
    reconstructionVolumePixelSpacing[0] = reconstrucionPixelSpacingResult.value()[2];
    reconstructionVolumePixelSpacing[1] = reconstrucionPixelSpacingResult.value()[1];
    reconstructionVolumePixelSpacing[2] = reconstrucionPixelSpacingResult.value()[0];

    auto centerXmlElement = gridXmlElement.firstChildElement( "center" );
    if( centerXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto reconstrucionCenterPositionResult = QStringToPositionIn3DWorld( centerXmlElement.firstChild().nodeValue() );
    if( reconstrucionCenterPositionResult.has_error() )
    {
        p_successfullConstruction = false;
        return;
    }
    reconstructionVolumeCenterPositionInWorld = reconstrucionCenterPositionResult.value();

    auto resolutionXmlElement = gridXmlElement.firstChildElement( "resolution" );
    if( resolutionXmlElement.isNull() )
    {
        p_successfullConstruction = false;
        return;
    }
    auto reconstructionVolumeSizeResult = QStringToSize3D( resolutionXmlElement.firstChild().nodeValue() );
    if( reconstructionVolumeSizeResult.has_error() )
    {
        p_successfullConstruction = false;
        return;
    }
    // z an x are inverted
    reconstructionVolumeSizeInPixels[0] = reconstructionVolumeSizeResult.value()[2];
    reconstructionVolumeSizeInPixels[1] = reconstructionVolumeSizeResult.value()[1];
    reconstructionVolumeSizeInPixels[2] = reconstructionVolumeSizeResult.value()[0];

    auto globalvolumeIndex = 0;
    for( auto zIndex = 0; zIndex < reconstructionVolumeSizeInPixels[2]; zIndex++ )
    {
        for( auto yIndex = 0; yIndex < reconstructionVolumeSizeInPixels[1]; yIndex++ )
        {
            for( auto xIndex = 0; xIndex < reconstructionVolumeSizeInPixels[0]; xIndex++ )
            {
                volumeIndices.push_back( globalvolumeIndex );
                globalvolumeIndex++;
            }
        }
    }

    reconstructionVolumeSizeInWorld[0] = static_cast<float>( reconstructionVolumeSizeInPixels[0] ) * reconstructionVolumePixelSpacing[0];
    reconstructionVolumeSizeInWorld[1] = static_cast<float>( reconstructionVolumeSizeInPixels[1] ) * reconstructionVolumePixelSpacing[1];
    reconstructionVolumeSizeInWorld[2] = static_cast<float>( reconstructionVolumeSizeInPixels[2] ) * reconstructionVolumePixelSpacing[2];

    volumeBLD[0] = reconstructionVolumeCenterPositionInWorld[0] - 0.5 * reconstructionVolumeSizeInWorld[0];
    volumeBLD[1] = reconstructionVolumeCenterPositionInWorld[1] - 0.5 * reconstructionVolumeSizeInWorld[1];
    volumeBLD[2] = reconstructionVolumeCenterPositionInWorld[2] - 0.5 * reconstructionVolumeSizeInWorld[2];

    volumeTRF[0] = volumeBLD[0] + reconstructionVolumeSizeInWorld[0];
    volumeTRF[1] = volumeBLD[1] + reconstructionVolumeSizeInWorld[0];
    volumeTRF[2] = volumeBLD[2] + reconstructionVolumeSizeInWorld[0];


    reconstructionXs.resize( reconstructionVolumeSizeInPixels[0] );
    reconstructionXs[0] = volumeBLD[0];
    for( auto xIndex{ 1 }; xIndex < reconstructionVolumeSizeInPixels[0]; xIndex++ )
    {
        reconstructionXs[xIndex] = reconstructionXs[xIndex - 1] + reconstructionVolumePixelSpacing[0];
    }

    reconstructionYs.resize( reconstructionVolumeSizeInPixels[1] );
    reconstructionYs[0] = volumeBLD[1];
    for( auto yIndex{ 1 }; yIndex < reconstructionVolumeSizeInPixels[1]; yIndex++ )
    {
        reconstructionYs[yIndex] = reconstructionYs[yIndex - 1] + reconstructionVolumePixelSpacing[1];
    }

    reconstructionZs.resize( reconstructionVolumeSizeInPixels[2] );
    reconstructionZs[0] = volumeBLD[2];
    for( auto zIndex{ 1 }; zIndex < reconstructionVolumeSizeInPixels[2]; zIndex++ )
    {
        reconstructionZs[zIndex] = reconstructionZs[zIndex - 1] + reconstructionVolumePixelSpacing[2];
    }
}

PixelOnProjection TomoGeometry::GetProjectionIndexAndPixelIndicesFromGlobalProjectionIndex( int p_globalIndex ) const
{
    PixelOnProjection pixelOnProjection;
    pixelOnProjection.projectionIndex = p_globalIndex / ( detectorsSizeInPixels.x * detectorsSizeInPixels.y );    // projection index
    pixelOnProjection.pixel.x = p_globalIndex % detectorsSizeInPixels.x;                                          // x
    pixelOnProjection.pixel.y = ( p_globalIndex / detectorsSizeInPixels.x ) % detectorsSizeInPixels.y;            // y
    return pixelOnProjection;
}

Voxel TomoGeometry::GetVoxelPositionGlobalProjectionIndex( int p_globalIndex ) const
{
    Voxel voxel;
    voxel.z = p_globalIndex / ( volumeSizeInPixels.x * volumeSizeInPixels.y );
    voxel.y = ( p_globalIndex / volumeSizeInPixels.x ) % volumeSizeInPixels.y;
    voxel.x = p_globalIndex % volumeSizeInPixels.x;
    return voxel;
}


PositionIn2DWorld TomoGeometry::GetPositionInWorldFromGlobalProjectionIndex( int p_globalIndex ) const
{
    auto pixelOnProjection = GetProjectionIndexAndPixelIndicesFromGlobalProjectionIndex( p_globalIndex );
    return GetPositionInWorldFromPixelOnProjection( pixelOnProjection );
}

PositionIn2DWorld TomoGeometry::GetPositionInWorldFromPixelOnProjection( const PixelOnProjection & p_pixelOnProjection ) const
{
    PositionIn2DWorld positionInWorld;
    positionInWorld.x = projectionsXPositionsInWorld[p_pixelOnProjection.projectionIndex][p_pixelOnProjection.pixel.x];
    positionInWorld.y = projectionsYPositionsInWorld[p_pixelOnProjection.projectionIndex][p_pixelOnProjection.pixel.y];
    return positionInWorld;
}


// general conversion rule :
//   - coordinates -> index              index = z * sizeY * sizeX + y * sizeX + x
int TomoGeometry::GetGlobalIndexFromVoxel( const Voxel & p_voxel ) const
{
    return p_voxel.z * volumeSizeInPixels.x * volumeSizeInPixels.y
           + p_voxel.y * volumeSizeInPixels.x
           + p_voxel.x;
}

// general conversion rule :
//   - coordinates -> index              index = z * sizeY * sizeX + y * sizeX + x
// z is the projection index
// y is p_pixelOnprojection.z
// x is p_pixelOnprojection.y
int TomoGeometry::GetGlobalIndexFromProjectionIndexAndPixelIndicesInRoi( const PixelOnProjection & p_pixelOnprojection ) const
{
    return p_pixelOnprojection.projectionIndex * detectorsSizeInPixels.x * detectorsSizeInPixels.y
           + p_pixelOnprojection.pixel.y * detectorsSizeInPixels.x
           + p_pixelOnprojection.pixel.x;
}


Voxel TomoGeometry::VoxelPositionFromReconstructionVolumePositionInWorld( const PositionIn3DWorld & p_position ) const
{
    Voxel voxel;
    voxel.x = static_cast<int>( ( p_position.x - volumeBLD.x ) / volumePixelSpacing.x );
    voxel.y = static_cast<int>( ( p_position.y - volumeBLD.y ) / volumePixelSpacing.y );
    voxel.z = static_cast<int>( ( p_position.z - volumeBLD.z ) / volumePixelSpacing.z );
    return voxel;
}


PositionIn3DWorld TomoGeometry::QStringToPositionIn3DWorld( const QString & p_positionString, bool & p_done ) const
{
    PositionIn3DWorld position;
    auto splittedString = p_positionString.split( " " );
    if( splittedString.size() != 3 )
    {
        p_done = false;
        return PositionIn3DWorld{};
    }

    auto conversionOK{ true };
    position.x = splittedString[0].toFloat( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return PositionIn3DWorld{};
    }
    position.y = splittedString[1].toFloat( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return PositionIn3DWorld{};
    }
    position.z = splittedString[2].toFloat( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return PositionIn3DWorld{};
    }

    p_done = true;
    return position;
}
Size3D TomoGeometry::QStringToSize3D( const QString & p_positionString, bool & p_done ) const
{
    Size3D volumeSize;
    auto splittedString = p_positionString.split( " " );
    if( splittedString.size() != 3 )
    {
        p_done = false;
        return Size3D{};
    }

    auto conversionOK{ true };
    volumeSize.x = splittedString[0].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return Size3D{};
    }
    volumeSize.y = splittedString[1].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return Size3D{};
    }
    volumeSize.z = splittedString[2].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return Size3D{};
    }
    p_done = true;
    return volumeSize;
}

QRect TomoGeometry::QStringToQRect( const QString & p_positionString, bool & p_done ) const
{
    auto splittedString = p_positionString.split( " " );
    if( splittedString.size() != 8 )
    {
        p_done = false;
        return QRect{};
    }

    auto conversionOK{ true };
    QPoint topLeft;
    QPoint bottomRight;
    auto xPosition = splittedString[0].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    topLeft.setX( xPosition );
    bottomRight.setX( xPosition );
    auto yPosition = splittedString[1].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    topLeft.setY( yPosition );
    bottomRight.setY( yPosition );
    xPosition = splittedString[2].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    if( xPosition < topLeft.x() )
    {
        topLeft.setX( xPosition );
    }
    if( xPosition > bottomRight.x() )
    {
        bottomRight.setX( xPosition );
    }
    yPosition = splittedString[3].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    if( yPosition < topLeft.y() )
    {
        topLeft.setY( yPosition );
    }
    if( yPosition > bottomRight.y() )
    {
        bottomRight.setY( yPosition );
    }
    xPosition = splittedString[4].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    if( xPosition < topLeft.x() )
    {
        topLeft.setX( xPosition );
    }
    if( xPosition > bottomRight.x() )
    {
        bottomRight.setX( xPosition );
    }
    yPosition = splittedString[5].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    if( yPosition < topLeft.y() )
    {
        topLeft.setY( yPosition );
    }
    if( yPosition > bottomRight.y() )
    {
        bottomRight.setY( yPosition );
    }
    xPosition = splittedString[6].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    if( xPosition < topLeft.x() )
    {
        topLeft.setX( xPosition );
    }
    if( xPosition > bottomRight.x() )
    {
        bottomRight.setX( xPosition );
    }
    yPosition = splittedString[7].toInt( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return QRect{};
    }
    if( yPosition < topLeft.y() )
    {
        topLeft.setY( yPosition );
    }
    if( yPosition > bottomRight.y() )
    {
        bottomRight.setY( yPosition );
    }
    return QRect{ topLeft, bottomRight };
}
