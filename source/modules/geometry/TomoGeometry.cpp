#pragma once

#include "modules/geometry/TomoGeometry.h"

#include "commons/GlobalUtils.h"
#include "commons/tinyXML/tinyxml2.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

TomoGeometry::TomoGeometry( const std::string & p_xmlGeometryFilePath )
{
    /***************************
    xml file check
    */
    tinyxml2::XMLDocument doc;
    auto xmlError = doc.LoadFile( p_xmlGeometryFilePath.c_str() );
    if( xmlError != tinyxml2::XMLError::XML_SUCCESS )
    {
        std::cout << "XML Error: " << xmlError << std::endl;
        return;
    }

    m_filePath = p_xmlGeometryFilePath;

    auto * rootElement = doc.RootElement();
    if( rootElement == nullptr )
    {
        std::cout << "XML Error: no root element" << std::endl;
        return;
    }


    /***************************
    Fulcrum (rotation center)
    */
    auto * rotationXmlElement = rootElement->FirstChildElement( "Rotation" );    // Grid represents the reconstruction geometry
    if( rotationXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Rotation" element)" << std::endl;
        return;
    }
    auto rotationCenterXmlElement = rotationXmlElement->FirstChildElement( "center" );
    if( rotationXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Rotation/center" element)" << std::endl;
        return;
    }

    auto rotationCenterString = std::string( rotationCenterXmlElement->GetText() );
    auto rotationCenterOptional = Position3D::FromString( rotationCenterString );
    if( !rotationCenterOptional.has_value() )
    {
        std::cout << R"(XML Error: "Rotation/center" element could not be parsed)" << std::endl;
        return;
    }
    m_fulcrum = rotationCenterOptional.value();


    /***************************
    Reconstruction volume
    */
    std::cout << "Reconstruction volume parsing" << std::endl;
    auto * gridXmlElement = rootElement->FirstChildElement( "Grid" );    // Grid represents the reconstruction geometry
    if( gridXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Grid" element)" << std::endl;
        return;
    }

    auto centerXmlElement = gridXmlElement->FirstChildElement( "center" );
    if( centerXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Grid/center" element)" << std::endl;
        return;
    }

    auto centerString = std::string( centerXmlElement->GetText() );
    auto centerOptional = Position3D::FromString( centerString );
    if( !centerOptional.has_value() )
    {
        std::cout << R"(XML Error: "Grid/center" element could not be parsed)" << std::endl;
        return;
    }
    auto center = centerOptional.value();

    auto voxelSpacingXmlElement = gridXmlElement->FirstChildElement( "scale" );
    if( voxelSpacingXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Grid/scale" element)" << std::endl;
        return;
    }

    auto voxelSpacingString = std::string( voxelSpacingXmlElement->GetText() );
    auto voxelSpacingOptional = Position3D::FromString( voxelSpacingString );
    if( !voxelSpacingOptional.has_value() )
    {
        std::cout << R"(XML Error: "Grid/scale" element could not be parsed)" << std::endl;
        return;
    }
    auto voxelSpacing = voxelSpacingOptional.value();

    VoxelSpacing voxelSpacingReordered( voxelSpacing.z, voxelSpacing.y, voxelSpacing.x );

    auto resolutionXmlElement = gridXmlElement->FirstChildElement( "resolution" );
    if( resolutionXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Grid/resolution" element)" << std::endl;
        return;
    }

    auto resolutionString = std::string( resolutionXmlElement->GetText() );
    auto resolutionOptional = Position3D::FromString( resolutionString );
    if( !resolutionOptional.has_value() )
    {
        std::cout << R"(XML Error: "Grid/resolution" element could not be parsed)" << std::endl;
        return;
    }
    auto resolution = resolutionOptional.value();

    Size3D resolutionReordered( resolution.z, resolution.y, resolution.x );

    WSize3D volumeSize( voxelSpacingReordered, resolutionReordered );

    Position3D volumeLeftBottomFrontPosition( -0.5 * volumeSize.x, -0.5 * volumeSize.y, -0.5 * volumeSize.z );

    m_volume = std::make_unique<TomoVolume>( volumeLeftBottomFrontPosition, resolutionReordered, voxelSpacingReordered );

    /***************************
    XRay sources positions
    */
    std::cout << "XRay sources positions parsing" << std::endl;
    auto xRayXmlElement = rootElement->FirstChildElement( "XRay" );
    if( xRayXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "XRay" element)" << std::endl;
        return;
    }

    auto sourcesXmlElement = xRayXmlElement->FirstChildElement( "sources" );
    if( sourcesXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "XRay/sources" element)" << std::endl;
        return;
    }
    // Get the first child of the component
    auto childSource = sourcesXmlElement->FirstChild();
    std::vector<Position3D> sourcePositions;
    // Read each child of the sources node
    while( childSource != nullptr )
    {
        auto childSourceElement = childSource->ToElement();
        auto sourcePositionString = std::string( childSourceElement->GetText() );
        auto sourcePositionOptional = Position3D::FromString( sourcePositionString );
        if( !sourcePositionOptional.has_value() )
        {
            std::cout << R"(XML Error: one among "XRay/sources" element could not be parsed)" << std::endl;
            return;
        }
        Position3D translatedSourcePosition( sourcePositionOptional.value().x - center.x, sourcePositionOptional.value().y - center.y, sourcePositionOptional.value().z - center.z );
        sourcePositions.push_back( translatedSourcePosition );

        // Next child
        childSource = childSourceElement->NextSibling();
    }
    if( sourcePositions.empty() )
    {
        std::cout << R"(XML Error: "XRay/sources" elements not parsed properly: is empty)" << std::endl;
        return;
    }


    /***************************
    Detectors positions and sizes
    */
    std::cout << "Detectors positions and sizes parsing" << std::endl;
    auto cameraXmlElement = rootElement->FirstChildElement( "Camera" );
    if( cameraXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Camera" element)" << std::endl;
        return;
    }

    auto totalWidthXmlElement = cameraXmlElement->FirstChildElement( "totalWidth" );
    if( totalWidthXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Camera/totalWidth" element)" << std::endl;
        return;
    }

    auto totalWidthString = std::string( totalWidthXmlElement->GetText() );
    float cameraWidth;
    try
    {
        cameraWidth = static_cast<float>( std::stod( totalWidthString ) );
    }
    catch( const std::invalid_argument & e )
    {
        std::cout << R"(XML Error: "Camera/totalWidth" element parsing problem: invalid argument )" << totalWidthString << std::endl;
        return;
    }
    catch( const std::out_of_range & e )
    {
        std::cout << R"(XML Error: "Camera/totalWidth" element parsing problem: out of range value )" << totalWidthString << std::endl;
        return;
    }

    if( AlmostEqualRelative( cameraWidth, 0.F ) )
    {
        std::cout << R"(XML Error: "Camera/totalWidth" element value problem: should not be zero )" << totalWidthString << std::endl;
        return;
    }

    auto totalHeightXmlElement = cameraXmlElement->FirstChildElement( "totalHeight" );
    if( totalHeightXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Camera/totalHeight" element)" << std::endl;
        return;
    }

    auto totalHeightString = std::string( totalHeightXmlElement->GetText() );
    float cameraHeight;
    try
    {
        cameraHeight = static_cast<float>( std::stod( totalHeightString ) );
    }
    catch( const std::invalid_argument & e )
    {
        std::cout << R"(XML Error: "Camera/totalHeight" element parsing problem: invalid argument )" << totalHeightString << std::endl;
        return;
    }
    catch( const std::out_of_range & e )
    {
        std::cout << R"(XML Error: "Camera/totalHeight" element parsing problem: out of range value )" << totalHeightString << std::endl;
        return;
    }

    if( AlmostEqualRelative( cameraHeight, 0.F ) )
    {
        std::cout << R"(XML Error: "Camera/totalHeight" element value problem: should not be zero )" << totalHeightString << std::endl;
        return;
    }
    WSize2D detectorsWSize( cameraWidth, cameraHeight );

    auto pixelWidthXmlElement = cameraXmlElement->FirstChildElement( "pixelWidth" );
    if( pixelWidthXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Camera/pixelWidth" element)" << std::endl;
        return;
    }

    auto pixelWidthString = std::string( pixelWidthXmlElement->GetText() );
    float pixelWidth;
    try
    {
        pixelWidth = static_cast<float>( std::stod( pixelWidthString ) );
    }
    catch( const std::invalid_argument & e )
    {
        std::cout << R"(XML Error: "Camera/pixelWidth" element parsing problem: invalid argument )" << pixelWidthString << std::endl;
        return;
    }
    catch( const std::out_of_range & e )
    {
        std::cout << R"(XML Error: "Camera/pixelWidth" element parsing problem: out of range value )" << pixelWidthString << std::endl;
        return;
    }

    if( pixelWidth == 0 )
    {
        std::cout << R"(XML Error: "Camera/pixelWidth" element value problem: should not be zero )" << pixelWidthString << std::endl;
        return;
    }

    auto pixelHeightXmlElement = cameraXmlElement->FirstChildElement( "pixelHeight" );
    if( pixelHeightXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Camera/pixelHeight" element)" << std::endl;
        return;
    }

    auto pixelHeightString = std::string( pixelHeightXmlElement->GetText() );
    float pixelHeight;
    try
    {
        pixelHeight = static_cast<float>( std::stod( pixelHeightString ) );
    }
    catch( const std::invalid_argument & e )
    {
        std::cout << R"(XML Error: "Camera/pixelHeight" element parsing problem: invalid argument )" << pixelHeightString << std::endl;
        return;
    }
    catch( const std::out_of_range & e )
    {
        std::cout << R"(XML Error: "Camera/pixelHeight" element parsing problem: out of range value )" << pixelHeightString << std::endl;
        return;
    }

    if( pixelHeight == 0 )
    {
        std::cout << R"(XML Error: "Camera/pixelHeight" element value problem: should not be zero )" << pixelHeightString << std::endl;
        return;
    }
    Size2D detectorsSize( pixelWidth, pixelHeight );
    auto referencesXmlElement = cameraXmlElement->FirstChildElement( "references" );
    if( referencesXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Camera/references" element)" << std::endl;
        return;
    }
    // Get the first child of the component
    auto detectorsPositionsChildReference = referencesXmlElement->FirstChild();
    std::vector<float> zDetectorPositions;
    std::vector<Position2D> detectorsBottomLeftPositions;
    // Read each child of the sources node
    while( detectorsPositionsChildReference != nullptr )
    {
        auto detectorsPositionsChildReferenceElement = detectorsPositionsChildReference->ToElement();
        auto detectorPositionString = std::string( detectorsPositionsChildReferenceElement->GetText() );
        auto detectorPositionOptional = Position3D::FromString( detectorPositionString );

        if( !detectorPositionOptional.has_value() )
        {
            std::cout << R"(XML Error: one among "Camera/references" element could not be parsed)" << std::endl;
            return;
        }

        auto detectorPositionInWorld = Position3D( detectorPositionOptional.value().x - center.x, detectorPositionOptional.value().y - center.y, detectorPositionOptional.value().z - center.z );

        detectorsBottomLeftPositions.push_back( Position2D( detectorPositionInWorld.x, detectorPositionInWorld.y ) );
        zDetectorPositions.push_back( detectorPositionInWorld.z );

        // Next child
        detectorsPositionsChildReference = detectorsPositionsChildReference->NextSibling();
    }
    if( detectorsBottomLeftPositions.empty() )
    {
        std::cout << R"(XML Error: detectors positions empty)" << std::endl;
        return;
    }

    m_projections = std::make_unique<TomoProjectionsSet>( detectorsBottomLeftPositions, detectorsWSize, detectorsSize );
    auto detectorsZCommonPosition = static_cast<float>( std::accumulate( zDetectorPositions.begin(), zDetectorPositions.end(), .0 ) ) / static_cast<float>( zDetectorPositions.size() );

    m_table = std::make_unique<TomoTable>( sourcePositions, detectorsZCommonPosition );

    /***************************
    Rois positions
    */
    std::cout << R"(Roi parsing)" << std::endl;
    auto radiosXmlElement = rootElement->FirstChildElement( "Radios" );
    if( radiosXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Radios" element)" << std::endl;
        return;
    }

    auto roisXmlElement = radiosXmlElement->FirstChildElement( "rois" );
    if( roisXmlElement == nullptr )
    {
        std::cout << R"(XML Error: no "Radios/rois" element)" << std::endl;
        return;
    }

    // Get the first child of the component
    auto roisChildReference = roisXmlElement->FirstChild();
    Pixel bottomLeftRoi( 0, 0 );
    Pixel topRightRoi( 0, 0 );
    int roiIndex = 0;
    // Read each child of the sources node
    while( roisChildReference != nullptr )
    {
        auto roisChildReferenceElement = roisChildReference->ToElement();
        auto roiString = std::string( roisChildReferenceElement->GetText() );
        auto roiCoordinates = glob::SentenceToWords( roiString );
        std::array<std::string, glob::TWO> bottomLeftCoordinatesString{ roiCoordinates[0], roiCoordinates[1] };
        std::array<std::string, glob::TWO> topRightCoordinatesString{ roiCoordinates[4], roiCoordinates[5] };

        auto currentRoiBottomLeftOptional = Pixel::FromStringArray( std::array<std::string, glob::TWO>{ roiCoordinates[0], roiCoordinates[1] } );
        if( !currentRoiBottomLeftOptional.has_value() )
        {
            std::cout << R"(XML Error: roi bottom left position parsing error)" << std::endl;
            return;
        }
        auto currentRoiTopRightOptional = Pixel::FromStringArray( std::array<std::string, glob::TWO>{ roiCoordinates[4], roiCoordinates[5] } );
        if( !currentRoiTopRightOptional.has_value() )
        {
            std::cout << R"(XML Error: roi top right position parsing error)" << std::endl;
            return;
        }
        if( roiIndex == 0 )
        {
            bottomLeftRoi = currentRoiBottomLeftOptional.value();
            topRightRoi = currentRoiTopRightOptional.value();
        }
        else
        {
            if( bottomLeftRoi != currentRoiBottomLeftOptional.value() )
            {
                std::cout << R"(XML Error: different roi bottom left positions )" << std::endl;
                return;
            }
            if( topRightRoi != currentRoiTopRightOptional.value() )
            {
                std::cout << R"(XML Error: different roi top right positions )" << std::endl;
                return;
            }
        }
        // Next child
        roisChildReference = roisChildReference->NextSibling();
        roiIndex++;
    }

    bottomLeftRoi.x = std::clamp( bottomLeftRoi.x, 0, detectorsSize.x - 1 );
    bottomLeftRoi.y = std::clamp( bottomLeftRoi.y, 0, detectorsSize.y - 1 );
    topRightRoi.x = std::clamp( topRightRoi.x, 0, detectorsSize.x - 1 );
    topRightRoi.y = std::clamp( topRightRoi.y, 0, detectorsSize.y - 1 );

    // bottomLeftRoi.x = 50;
    // bottomLeftRoi.y = 50;
    // topRightRoi.x = 3000;
    // topRightRoi.y = 3000;

    m_projectionROIsBLPixelPositionOnDetector = Pixel( bottomLeftRoi.x, bottomLeftRoi.y );

    std::vector<Position2D> projectionsRoisBottomLeftPositions;
    for( auto projectionIndex = 0; projectionIndex < m_projections->GetNProjections(); projectionIndex++ )
    {
        projectionsRoisBottomLeftPositions.push_back( m_projections->GetPosition( projectionIndex, m_projectionROIsBLPixelPositionOnDetector ) );
    }

    m_projectionsRois = std::make_unique<TomoProjectionsSet>( projectionsRoisBottomLeftPositions, Size2D( topRightRoi.x - bottomLeftRoi.x + 1, topRightRoi.y - bottomLeftRoi.y + 1 ), m_projections->GetPixelSpacing() );

    m_isValid = true;
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
    return m_volume->GetBottomLeftFront();
}
Position3D TomoGeometry::volumeTRB() const
{
    return m_volume->GetTopRightBack();
}
float TomoGeometry::volumeSize() const
{
    return m_volume->GetSize();
}
std::vector<float> TomoGeometry::volumeZs() const
{
    return m_volume->GetZPositions();
}
std::vector<float> TomoGeometry::volumeYs() const
{
    return m_volume->GetYPositions();
}
std::vector<float> TomoGeometry::volumeXs() const
{
    return m_volume->GetXPositions();
}
std::vector<int> TomoGeometry::volumeVoxelsIndices() const
{
    return m_volume->GetVoxelsIndices();
}
std::vector<Position3D> TomoGeometry::volumeGridPositions() const
{
    return m_volume->GetGridPositions();
}

std::vector<float> TomoGeometry::sourcesYPositions() const
{
    return m_table->GetSourcesYPositions();
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
std::vector<Position2D> TomoGeometry::projectionsBottomLeftPositions() const
{
    return m_projections->GetBottomLeftPositions();
}
std::vector<Position2D> TomoGeometry::projectionsPositions() const
{
    return m_projections->GetProjectionPositions();
}
Size2D TomoGeometry::projectionsSize() const
{
    return m_projections->GetSize();
}
WSize2D TomoGeometry::projectionsWSize() const
{
    return m_projections->GetWSize();
}
std::vector<std::vector<float>> TomoGeometry::projectionsXPositions() const
{
    return m_projections->GetXPositions();
}
std::vector<std::vector<float>> TomoGeometry::projectionsYPositions() const
{
    return m_projections->GetYPositions();
}
PixelSpacing TomoGeometry::projectionsPixelSpacing() const
{
    return m_projections->GetPixelSpacing();
}

// mainly used for parallelization
std::vector<int> TomoGeometry::projectionsPositionsIndices() const
{
    return m_projections->GetProjectionPixelsIndices();
}
// std::vector<PixelOnProjection> TomoGeometry::projectionIndiceAndPixels() const
//{
//     return m_projections->GetProjectionIndiceAndPixels();
// }

int TomoGeometry::nbProjectionsRois() const
{
    return m_projectionsRois->GetNProjections();
}
std::vector<Position2D> TomoGeometry::projectionsRoisBottomLeftPositions() const
{
    return m_projectionsRois->GetBottomLeftPositions();
}
std::vector<Position2D> TomoGeometry::projectionsRoisPositions() const
{
    return m_projectionsRois->GetProjectionPositions();
}
Size2D TomoGeometry::projectionsRoisSize() const
{
    return m_projectionsRois->GetSize();
}
WSize2D TomoGeometry::projectionsRoisWSize() const
{
    return m_projectionsRois->GetWSize();
}
std::vector<std::vector<float>> TomoGeometry::projectionsRoisXPositions() const
{
    return m_projectionsRois->GetXPositions();
}
std::vector<std::vector<float>> TomoGeometry::projectionsRoisYPositions() const
{
    return m_projectionsRois->GetYPositions();
}
PixelSpacing TomoGeometry::projectionsRoisPixelSpacing() const
{
    return m_projectionsRois->GetPixelSpacing();
}

// mainly used for parallelization
std::vector<int> TomoGeometry::projectionsRoisPositionsIndices() const
{
    return m_projectionsRois->GetProjectionPixelsIndices();
}
// std::vector<PixelOnProjection> TomoGeometry::projectionRoisIndiceAndPixels() const
//{
//     return m_projectionsRois->GetProjectionIndiceAndPixels();
// }

void TomoGeometry::PerformCheatingAdaptationsForDemonstration( std::vector<int> const & p_dataIndicesToRemove ) const
{
    if( !p_dataIndicesToRemove.empty() )
    {
        m_table->RemoveSource( p_dataIndicesToRemove );
        m_projections->RemoveProjection( p_dataIndicesToRemove );
        m_projectionsRois->RemoveProjection( p_dataIndicesToRemove );
    }

    // volume resolution and size change
    auto resolution = m_volume->GetSize3D();
    resolution.z = 100;
    m_volume->SetSizeAndUpdateWSize( resolution );

    // projections resolution and pixel spacing change
    m_projections->SetSizeAndUpdatePixelSpacing( Size2D( 3048, 3048 ) );

    m_projectionsRois->SetSizeAndUpdateWSize( Size2D( 3048, 3048 ) );
}