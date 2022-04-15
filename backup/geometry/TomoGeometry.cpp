#pragma once

#include "modules/geometry/TomoGeometry.h"

#include <QDebug>
#include <QDomDocument>
#include <QFile>

#include <cmath>

TomoGeometry::TomoGeometry( const QString & p_xmlGeometryFilePath )
{
    /***************************
    xml file check
    */
    QFile xmlGeometryFile{ p_xmlGeometryFilePath };
    QDomDocument xmlDoc( "manifest" );
    if( !xmlGeometryFile.exists() && !xmlGeometryFile.open( QIODevice::ReadOnly ) )
    {
        return;
    }
    if( !xmlDoc.setContent( &xmlGeometryFile ) )
    {
        return;
    }
    xmlGeometryFile.close();

    auto geometryElem = xmlDoc.documentElement();
    if( geometryElem.isNull() )
    {
        return;
    }

    m_filePath = p_xmlGeometryFilePath;

    /***************************
    Reconstruction volume
    */
    auto gridXmlElement = geometryElem.firstChildElement( "Grid" );    // Grid represents the reconstruction geometry
    if( gridXmlElement.isNull() )
    {
        return;
    }

    auto centerXmlElement = gridXmlElement.firstChildElement( "center" );
    if( centerXmlElement.isNull() )
    {
        return;
    }
    bool successfullParse;
    auto volumeCenterPosition = QStringToPosition3D( centerXmlElement.firstChild().nodeValue(), successfullParse );
    if( !successfullParse )
    {
        return;
    }

    auto scaleXmlElement = gridXmlElement.firstChildElement( "scale" );
    if( scaleXmlElement.isNull() )
    {
        return;
    }
    auto reconstructionVoxelSpacing = QStringToPosition3D( scaleXmlElement.firstChild().nodeValue(), successfullParse );
    if( !successfullParse )
    {
        return;
    }
    // z an x are inverted
    VoxelSpacing reOrderedVoxelSpacing( reconstructionVoxelSpacing.z, reconstructionVoxelSpacing.y, reconstructionVoxelSpacing.x );


    auto resolutionXmlElement = gridXmlElement.firstChildElement( "resolution" );
    if( resolutionXmlElement.isNull() )
    {
        return;
    }
    auto reconstructionVolumeSize = QStringToSize3D( resolutionXmlElement.firstChild().nodeValue(), successfullParse );
    if( !successfullParse )
    {
        return;
    }
    Size3D reorderedVolumeSize( reconstructionVolumeSize.z, reconstructionVolumeSize.y, reconstructionVolumeSize.x );

    auto volumeWSize = ComputeWSize3D( reorderedVolumeSize, reOrderedVoxelSpacing );

    Position3D volumeLeftBottomFrontPosition;
    volumeLeftBottomFrontPosition.x = -volumeWSize.x / 2.;
    volumeLeftBottomFrontPosition.y = -volumeWSize.y / 2.;
    volumeLeftBottomFrontPosition.z = -volumeWSize.z / 2.;

    m_volume = std::make_unique<TomoVolume>( volumeLeftBottomFrontPosition, reorderedVolumeSize, reOrderedVoxelSpacing );


    /***************************
    Fulcrum position
    */
    auto rotationXmlElement = geometryElem.firstChildElement( "Rotation" );
    if( rotationXmlElement.isNull() )
    {
        return;
    }

    auto rotationCenterXmlElement = rotationXmlElement.firstChildElement( "center" );
    if( rotationCenterXmlElement.isNull() )
    {
        return;
    }


    auto positionString = rotationCenterXmlElement.firstChild().nodeValue();
    auto rotationCenter = QStringToPosition3D( positionString, successfullParse ) - volumeCenterPosition;
    if( !successfullParse )
    {
        return;
    }


    /***************************
    XRay sources positions
    */
    auto xRayXmlElement = geometryElem.firstChildElement( "XRay" );
    if( xRayXmlElement.isNull() )
    {
        return;
    }
    auto sourcesXmlElement = xRayXmlElement.firstChildElement( "sources" );
    if( sourcesXmlElement.isNull() )
    {
        return;
    }
    // Get the first child of the component
    auto childSource = sourcesXmlElement.firstChild().toElement();
    std::vector<Position3D> sourcePositions;
    // Read each child of the sources node
    while( !childSource.isNull() )
    {
        auto sourcePositionString = childSource.firstChild().nodeValue();
        auto sourcePositionInWorld = QStringToPosition3D( sourcePositionString, successfullParse ) - volumeCenterPosition;
        if( !successfullParse )
        {
            return;
        }
        sourcePositions.push_back( sourcePositionInWorld );
        // Next child
        childSource = childSource.nextSibling().toElement();
    }
    if( sourcePositions.empty() )
    {
        return;
    }

    /***************************
    Detectors positions and sizes
    */
    auto cameraXmlElement = geometryElem.firstChildElement( "Camera" );
    if( cameraXmlElement.isNull() )
    {
        return;
    }
    auto totalWidthXmlElement = cameraXmlElement.firstChildElement( "totalWidth" );
    if( totalWidthXmlElement.isNull() )
    {
        return;
    }
    auto conversionOK{ true };
    auto cameraWidth = totalWidthXmlElement.firstChild().nodeValue().toFloat( &conversionOK );
    if( !conversionOK || AlmostEqualRelative( cameraWidth, 0.F ) )
    {
        return;
    }

    auto totalHeightXmlElement = cameraXmlElement.firstChildElement( "totalHeight" );
    if( totalHeightXmlElement.isNull() )
    {
        return;
    }
    auto cameraHeight = totalHeightXmlElement.firstChild().nodeValue().toFloat( &conversionOK );
    if( !conversionOK || AlmostEqualRelative( cameraHeight, 0.f ) )
    {
        return;
    }

    WSize2D detectorsWSize( cameraWidth, cameraHeight );

    auto pixelWidthXmlElement = cameraXmlElement.firstChildElement( "pixelWidth" );
    if( pixelWidthXmlElement.isNull() )
    {
        return;
    }
    auto pixelWidth = pixelWidthXmlElement.firstChild().nodeValue().toInt( &conversionOK );
    if( !conversionOK || pixelWidth == 0 )
    {
        return;
    }
    auto pixelHeightXmlElement = cameraXmlElement.firstChildElement( "pixelHeight" );
    if( pixelHeightXmlElement.isNull() )
    {
        return;
    }
    auto pixelHeight = pixelHeightXmlElement.firstChild().nodeValue().toInt( &conversionOK );
    if( !conversionOK || pixelHeight == 0 )
    {
        return;
    }
    Size2D detectorsSize( pixelWidth, pixelHeight );

    auto referencesXmlElement = cameraXmlElement.firstChildElement( "references" );
    if( referencesXmlElement.isNull() )
    {
        return;
    }
    // Get the first child of the component
    auto childReference = referencesXmlElement.firstChild().toElement();
    std::vector<float> zDetectorPositions;
    std::vector<Position2D> detectorsBottomLeftPositions;
    // Read each child of the sources node
    while( !childReference.isNull() )
    {
        auto detectorPositionString = childReference.firstChild().nodeValue();
        auto detectorPositionInWorld = QStringToPosition3D( detectorPositionString, successfullParse ) - volumeCenterPosition;
        if( !successfullParse )
        {
            return;
        }
        detectorsBottomLeftPositions.push_back( Position2D( detectorPositionInWorld.x, detectorPositionInWorld.y ) );
        zDetectorPositions.push_back( detectorPositionInWorld.z );

        // Next child
        childReference = childReference.nextSibling().toElement();
    }
    if( detectorsBottomLeftPositions.empty() )
    {
        return;
    }

    m_projections = std::make_unique<TomoProjectionsSet>( detectorsBottomLeftPositions, detectorsWSize, detectorsSize );
    auto detectorsZCommonPosition = static_cast<float>( std::accumulate( zDetectorPositions.begin(), zDetectorPositions.end(), .0 ) ) / static_cast<float>( zDetectorPositions.size() );

    m_table = std::make_unique<TomoTable>( rotationCenter, sourcePositions, detectorsZCommonPosition );


    /***************************
    Projections ROIs
    */
    auto radiosXmlElement = geometryElem.firstChildElement( "Radios" );
    if( radiosXmlElement.isNull() )
    {
        return;
    }

    auto roisXmlElement = radiosXmlElement.firstChildElement( "rois" );
    if( roisXmlElement.isNull() )
    {
        return;
    }
    // Get the first child of the component
    auto childRois = roisXmlElement.firstChild().toElement();

    auto roiIndex{ 0 };
    QRect uniqueRoi;
    // Read each child of the sources node
    while( !childRois.isNull() )
    {
        auto roiString = childRois.firstChild().nodeValue();
        auto roiRect = QStringToQRect( roiString, successfullParse );
        if( !successfullParse )
        {
            return;
        }
        if( roiIndex == 0 )
        {
            uniqueRoi = roiRect;
        }
        else
        {
            if( roiRect != uniqueRoi )
            {
                qWarning() << " some Rois of projections are different";
            }
        }
        // Next child
        roiIndex++;
        childRois = childRois.nextSibling().toElement();
    }
    m_projectionROIsBLPixelPositionOnDetector = Pixel( uniqueRoi.x(), uniqueRoi.y() );

    std::vector<Position2D> projectionsRoisBottomLeftPositions( m_projections->GetNProjections() );
    for( auto projectionIndex = 0; projectionIndex < m_projections->GetNProjections(); projectionIndex++ )
    {
        projectionsRoisBottomLeftPositions[projectionIndex] = m_projections->GetPosition( projectionIndex, m_projectionROIsBLPixelPositionOnDetector );
    }

    m_projectionsRois = std::make_unique<TomoProjectionsSet>( projectionsRoisBottomLeftPositions, Size2D( uniqueRoi.width(), uniqueRoi.height() ), m_projections->GetPixelSpacing() );

    m_isValid = true;
}


Position3D TomoGeometry::QStringToPosition3D( const QString & p_positionString, bool & p_done ) const
{
    Position3D position;
    auto splittedString = p_positionString.split( " " );
    if( splittedString.size() != 3 )
    {
        p_done = false;
        return Position3D{};
    }

    auto conversionOK{ true };
    position.x = splittedString[0].toFloat( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return Position3D{};
    }
    position.y = splittedString[1].toFloat( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return Position3D{};
    }
    position.z = splittedString[2].toFloat( &conversionOK );
    if( !conversionOK )
    {
        p_done = false;
        return Position3D{};
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

// getters for volume
Size3D TomoGeometry::volumeSize3D() const
{
    return m_volume->GetSize3D();
}
VoxelSpacing TomoGeometry::volumeVoxelSpacing() const
{
    return m_volume->GetVoxelSpacing();
}
WSize3D TomoGeometry::volumeWSize3D() const
{
    return m_volume->GetWSize3D();
}
Position3D TomoGeometry::volumeBLF() const
{
    return m_volume->GetBLF();
}
Position3D TomoGeometry::volumeTRB() const
{
    return m_volume->GetTRB();
}
float TomoGeometry::volumeSize() const
{
    return m_volume->GetSize();
}
const std::vector<float> * TomoGeometry::volumeZsPtr() const
{
    return m_volume->GetZPositionsPtr();
}
const std::vector<float> * TomoGeometry::volumeYsPtr() const
{
    return m_volume->GetYPositionsPtr();
}
const std::vector<float> * TomoGeometry::volumeXsPtr() const
{
    return m_volume->GetXPositionsPtr();
}
const std::vector<int> * TomoGeometry::volumeVoxelsIndicesPtr() const
{
    return m_volume->GetVoxelsIndicesPtr();
}
const std::vector<Position3D> * TomoGeometry::volumeGridPositionsPtr() const
{
    return m_volume->GetGridPositionsPtr();
}


// getters for table
Position3D TomoGeometry::rotationCenter() const
{
    return m_table->GetRotationCenter();
}

const std::vector<float> * TomoGeometry::sourcesYPositionsPtr() const
{
    return m_table->GetSourcesYPositionsPtr();
}
float TomoGeometry::sourcesXCommonPosition() const
{
    return m_table->GetSourcesXCommonPosition();
}
float TomoGeometry::sourcesZCommonPosition() const
{
    return m_table->GetSourcesZCommonPosition();
}
float TomoGeometry::detectorsZCommonPosition() const
{
    return m_table->GetDetectorsZCommonPosition();
}
float TomoGeometry::sid() const
{
    return std::abs( m_table->GetDetectorsZCommonPosition() - m_table->GetSourcesZCommonPosition() );
}

// detectors geometric features
int TomoGeometry::nbProjections() const
{
    return m_projections->GetNProjections();
}
const std::vector<Position2D> * TomoGeometry::projectionsBottomLeftPositionsPtr() const
{
    return m_projections->GetBottomLeftPositionsPtr();
}
const std::vector<Position2D> * TomoGeometry::projectionsPositionsPtr() const
{
    return m_projections->GetProjectionPositionsPtr();
}
Size2D TomoGeometry::projectionsSize() const
{
    return m_projections->GetSize();
}
WSize2D TomoGeometry::projectionsWSize() const
{
    return m_projections->GetWSize();
}
const std::vector<std::vector<float>> * TomoGeometry::projectionsXPositionsPtr() const
{
    return m_projections->GetXPositionsPtr();
}
const std::vector<std::vector<float>> * TomoGeometry::projectionsYPositionsPtr() const
{
    return m_projections->GetYPositionsPtr();
}
PixelSpacing TomoGeometry::projectionsPixelSpacing() const
{
    return m_projections->GetPixelSpacing();
}

// mainly used for parallelization
const std::vector<int> * TomoGeometry::projectionsPositionsIndicesPtr() const
{
    return m_projections->GetProjectionPixelsIndicesPtr();
}
const std::vector<PixelOnProjection> * TomoGeometry::projectionIndiceAndPixelsPtr() const
{
    return m_projections->GetProjectionIndiceAndPixelsPtr();
}


int TomoGeometry::nbProjectionsRois() const
{
    return m_projectionsRois->GetNProjections();
}
const std::vector<Position2D> * TomoGeometry::projectionsRoisBottomLeftPositionsPtr() const
{
    return m_projectionsRois->GetBottomLeftPositionsPtr();
}
const std::vector<Position2D> * TomoGeometry::projectionsRoisPositionsPtr() const
{
    return m_projectionsRois->GetProjectionPositionsPtr();
}
Size2D TomoGeometry::projectionsRoisSize() const
{
    return m_projectionsRois->GetSize();
}
WSize2D TomoGeometry::projectionsRoisWSize() const
{
    return m_projectionsRois->GetWSize();
}
const std::vector<std::vector<float>> * TomoGeometry::projectionsRoisXPositionsPtr() const
{
    return m_projectionsRois->GetXPositionsPtr();
}
const std::vector<std::vector<float>> * TomoGeometry::projectionsRoisYPositionsPtr() const
{
    return m_projectionsRois->GetYPositionsPtr();
}
PixelSpacing TomoGeometry::projectionsRoisPixelSpacing() const
{
    return m_projectionsRois->GetPixelSpacing();
}

// mainly used for parallelization
const std::vector<int> * TomoGeometry::projectionsRoisPositionsIndicesPtr() const
{
    return m_projectionsRois->GetProjectionPixelsIndicesPtr();
}
const std::vector<PixelOnProjection> * TomoGeometry::projectionRoisIndiceAndPixelsPtr() const
{
    return m_projectionsRois->GetProjectionIndiceAndPixelsPtr();
}
