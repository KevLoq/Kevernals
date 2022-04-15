#include "modules/geometry/TomoGeometryParser.h"

#include "modules/geometry/TomoGeometryErrorCode.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QString>

Result<Quadrilateral2D> QStringToQuadrilateral2D( const QString & p_positionString )
{
    Quadrilateral2D quadrilateral;
    auto splittedString = p_positionString.split( " " );
    if( splittedString.size() != 8 )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }

    auto conversionOK{ true };
    quadrilateral[0][0] = splittedString[0].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    quadrilateral[0][1] = splittedString[1].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    quadrilateral[1][0] = splittedString[2].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    quadrilateral[1][1] = splittedString[3].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    quadrilateral[2][0] = splittedString[4].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    quadrilateral[2][1] = splittedString[5].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    quadrilateral[3][0] = splittedString[6].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    quadrilateral[3][1] = splittedString[7].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError );
    }
    return quadrilateral;
}
Result<PositionIn3DWorld> QStringToPositionIn3DWorld( const QString & p_positionString )
{
    PositionIn3DWorld position;
    auto splittedString = p_positionString.split( " " );
    if( splittedString.size() != 3 )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }

    auto conversionOK{ true };
    position[0] = splittedString[0].toDouble( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }
    position[1] = splittedString[1].toDouble( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }
    position[2] = splittedString[2].toDouble( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }

    return position;
}
Result<Size3D> QStringToSize3D( const QString & p_positionString )
{
    Size3D volumeSize;
    auto splittedString = p_positionString.split( " " );
    if( splittedString.size() != 3 )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }

    auto conversionOK{ true };
    volumeSize[0] = splittedString[0].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }
    volumeSize[1] = splittedString[1].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }
    volumeSize[2] = splittedString[2].toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError );
    }

    return volumeSize;
}


Result<TomoGeometry> TomoGeometryParser::ParseXmlFile( const QString & p_xmlGeometryFilePath )
{
    /***************************
    xml file check
    */
    QFile xmlGeometryFile{ p_xmlGeometryFilePath };
    QDomDocument xmlDoc( "manifest" );
    if( !xmlGeometryFile.exists() && !xmlGeometryFile.open( QIODevice::ReadOnly ) )
    {
        return make_error_code( TomoGeometryErrorCode::FileToParseError );
    }
    if( !xmlDoc.setContent( &xmlGeometryFile ) )
    {
        return make_error_code( TomoGeometryErrorCode::FileToParseError );
    }
    xmlGeometryFile.close();

    auto geometryElem = xmlDoc.documentElement();
    if( geometryElem.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::FileToParseError );
    }

    TomoGeometry geometry;
    geometry.filePath = p_xmlGeometryFilePath;
    /***************************
    Fulcrum position
    */
    auto rotationXmlElement = geometryElem.firstChildElement( "Rotation" );
    if( rotationXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::FulcrumParsingError );
    }

    auto rotationCenterXmlElement = rotationXmlElement.firstChildElement( "center" );
    if( rotationCenterXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::FulcrumParsingError );
    }


    auto positionString = rotationCenterXmlElement.firstChild().nodeValue();
    auto rotationCenterPositionInWorldResult = QStringToPositionIn3DWorld( positionString );
    if( rotationCenterPositionInWorldResult.has_error() )
    {
        qCritical() << QString::fromStdString( rotationCenterPositionInWorldResult.error().message() );
        return make_error_code( TomoGeometryErrorCode::FulcrumParsingError );
    }
    geometry.rotationCenterInWorld = rotationCenterPositionInWorldResult.value();


    /***************************
    XRay sources positions
    */
    auto xRayXmlElement = geometryElem.firstChildElement( "XRay" );
    if( xRayXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::XraySourcesPositionParsingError );
    }
    auto sourcesXmlElement = xRayXmlElement.firstChildElement( "sources" );
    if( sourcesXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::XraySourcesPositionParsingError );
    }
    // Get the first child of the component
    auto childSource = sourcesXmlElement.firstChild().toElement();
    geometry.xRaySourcesPositionsInWorld.clear();
    // Read each child of the sources node
    while( !childSource.isNull() )
    {
        auto sourcePositionString = childSource.firstChild().nodeValue();
        auto sourcePositionInWorldResult = QStringToPositionIn3DWorld( sourcePositionString );
        if( sourcePositionInWorldResult.has_error() )
        {
            qCritical() << QString::fromStdString( sourcePositionInWorldResult.error().message() );
            return make_error_code( TomoGeometryErrorCode::XraySourcesPositionParsingError );
        }
        geometry.xRaySourcesPositionsInWorld.push_back( sourcePositionInWorldResult.value() );
        // Next child
        childSource = childSource.nextSibling().toElement();
    }

    /***************************
    Projections positions
    */
    auto cameraXmlElement = geometryElem.firstChildElement( "Camera" );
    if( cameraXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    auto totalWidthXmlElement = cameraXmlElement.firstChildElement( "totalWidth" );
    if( totalWidthXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    auto conversionOK{ true };
    auto cameraWidth = totalWidthXmlElement.firstChild().nodeValue().toDouble( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    auto totalHeightXmlElement = cameraXmlElement.firstChildElement( "totalHeight" );
    if( totalHeightXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    auto cameraHeight = totalHeightXmlElement.firstChild().nodeValue().toDouble( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }

    auto referencesXmlElement = cameraXmlElement.firstChildElement( "references" );
    if( referencesXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    // Get the first child of the component
    auto childReference = referencesXmlElement.firstChild().toElement();

    geometry.detectorCenterPositionsInWorld.clear();
    geometry.detectorBLPositionsInWorld.clear();

    // Read each child of the sources node
    while( !childReference.isNull() )
    {
        auto detectorPositionString = childReference.firstChild().nodeValue();
        auto detectorPositionInWorldResult = QStringToPositionIn3DWorld( detectorPositionString );
        if( detectorPositionInWorldResult.has_error() )
        {
            qCritical() << QString::fromStdString( detectorPositionInWorldResult.error().message() );
            return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
        }
        geometry.detectorCenterPositionsInWorld.push_back( detectorPositionInWorldResult.value() );
        geometry.detectorBLPositionsInWorld.push_back( detectorPositionInWorldResult.value() );

        // We use the central position of detector
        geometry.detectorCenterPositionsInWorld.back()[0] += cameraWidth / 2.;
        geometry.detectorCenterPositionsInWorld.back()[1] += cameraHeight / 2.;

        // Next child
        childReference = childReference.nextSibling().toElement();
    }

    geometry.nbProjections = geometry.detectorCenterPositionsInWorld.size();

    auto pixelWidthXmlElement = cameraXmlElement.firstChildElement( "pixelWidth" );
    if( pixelWidthXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    auto pixelWidth = pixelWidthXmlElement.firstChild().nodeValue().toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    auto pixelHeightXmlElement = cameraXmlElement.firstChildElement( "pixelHeight" );
    if( pixelHeightXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    auto pixelHeight = pixelHeightXmlElement.firstChild().nodeValue().toInt( &conversionOK );
    if( !conversionOK )
    {
        return make_error_code( TomoGeometryErrorCode::DetectorPositionsParsingError );
    }
    geometry.projectionPixelSpacing[0] = cameraWidth / static_cast<double>( pixelWidth );
    geometry.projectionPixelSpacing[1] = cameraHeight / static_cast<double>( pixelHeight );
    geometry.projectionsSizeInPixels[0] = pixelWidth;
    geometry.projectionsSizeInPixels[1] = pixelHeight;


    /***************************
    Projections ROIs
    */
    auto radiosXmlElement = geometryElem.firstChildElement( "Radios" );
    if( radiosXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::ProjectionRoisParsingError );
    }

    auto roisXmlElement = radiosXmlElement.firstChildElement( "rois" );
    if( roisXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::ProjectionRoisParsingError );
    }
    // Get the first child of the component
    auto childRois = roisXmlElement.firstChild().toElement();

    auto roiIndex{ 0 };
    // Read each child of the sources node
    while( !childRois.isNull() )
    {
        auto roiString = childRois.firstChild().nodeValue();
        auto roiResult = QStringToQuadrilateral2D( roiString );
        if( roiResult.has_error() )
        {
            qCritical() << QString::fromStdString( roiResult.error().message() );
            return make_error_code( TomoGeometryErrorCode::ProjectionRoisParsingError );
        }
        if( roiIndex == 0 )
        {
            geometry.projectionsRoi = roiResult.value();
        }
        else
        {
            if( geometry.projectionsRoi != roiResult.value() )
            {
                qWarning() << " some Rois of projections are different";
            }
        }
        // Next child
        roiIndex++;
        childRois = childRois.nextSibling().toElement();
    }


    geometry.minXProjectionRoi = geometry.projectionsSizeInPixels[0];
    geometry.maxXProjectionRoi = 0;
    geometry.minYProjectionRoi = geometry.projectionsSizeInPixels[1];
    geometry.maxYProjectionRoi = 0;
    for( const auto & vertex : geometry.projectionsRoi )
    {
        if( vertex[0] < geometry.minXProjectionRoi )
        {
            geometry.minXProjectionRoi = vertex[0];
        }
        if( vertex[0] > geometry.maxXProjectionRoi )
        {
            geometry.maxXProjectionRoi = vertex[0];
        }

        if( vertex[1] < geometry.minYProjectionRoi )
        {
            geometry.minYProjectionRoi = vertex[1];
        }
        if( vertex[1] > geometry.maxYProjectionRoi )
        {
            geometry.maxYProjectionRoi = vertex[1];
        }
    }
    if( geometry.minXProjectionRoi >= geometry.maxXProjectionRoi || geometry.minYProjectionRoi >= geometry.maxYProjectionRoi )
    {
        auto errorCode = make_error_code( TomoGeometryErrorCode::ProjectionRoisParsingError );
        qCritical() << QString::fromStdString( errorCode.message() );
        return errorCode;
    }
    geometry.projectionRoiXSize = geometry.maxXProjectionRoi + 1 - geometry.minXProjectionRoi;
    geometry.projectionRoiYSize = geometry.maxYProjectionRoi + 1 - geometry.minYProjectionRoi;
    geometry.roiXPositionsInWorld.resize( geometry.nbProjections );
    geometry.roiYPositionsInWorld.resize( geometry.nbProjections );

    QVector<QVector<float>> roiYPositionsInWorld;
    for( auto projectionIndex = 0; projectionIndex < geometry.detectorCenterPositionsInWorld.size(); projectionIndex++ )
    {
        auto blProjectionInWorldX = geometry.detectorBLPositionsInWorld[projectionIndex][0];
        geometry.roiXPositionsInWorld[projectionIndex].resize( geometry.projectionRoiXSize );
        geometry.roiXPositionsInWorld[projectionIndex][0] = blProjectionInWorldX + static_cast<float>( geometry.minXProjectionRoi ) * geometry.projectionPixelSpacing[0];
        for( auto pixelX = 1; pixelX < geometry.projectionRoiXSize; pixelX++ )
        {
            geometry.roiXPositionsInWorld[projectionIndex][pixelX] = geometry.roiXPositionsInWorld[projectionIndex][pixelX - 1] + geometry.projectionPixelSpacing[0];
        }

        auto blProjectionInWorldY = geometry.detectorBLPositionsInWorld[projectionIndex][1];
        geometry.roiYPositionsInWorld[projectionIndex].resize( geometry.projectionRoiYSize );
        geometry.roiYPositionsInWorld[projectionIndex][0] = blProjectionInWorldY + static_cast<float>( geometry.minYProjectionRoi ) * geometry.projectionPixelSpacing[1];
        for( auto pixelY = 1; pixelY < geometry.projectionRoiYSize; pixelY++ )
        {
            geometry.roiYPositionsInWorld[projectionIndex][pixelY] = geometry.roiYPositionsInWorld[projectionIndex][pixelY - 1] + geometry.projectionPixelSpacing[1];
        }
    }

    auto globalProjectionIndex = 0;
    for( auto projectionIndex = 0; projectionIndex < geometry.detectorCenterPositionsInWorld.size(); projectionIndex++ )
    {
        for( auto pixelY = 0; pixelY < geometry.projectionRoiYSize; pixelY++ )
        {
            for( auto pixelX = 0; pixelX < geometry.projectionRoiXSize; pixelX++ )
            {
                geometry.projectionIndices.push_back( globalProjectionIndex );
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
        return make_error_code( TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError );
    }

    auto scaleXmlElement = gridXmlElement.firstChildElement( "scale" );
    if( scaleXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError );
    }
    auto reconstrucionPixelSpacingResult = QStringToPositionIn3DWorld( scaleXmlElement.firstChild().nodeValue() );
    if( reconstrucionPixelSpacingResult.has_error() )
    {
        qCritical() << QString::fromStdString( reconstrucionPixelSpacingResult.error().message() );
        return make_error_code( TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError );
    }
    // z an x are inverted
    geometry.reconstructionVolumePixelSpacing[0] = reconstrucionPixelSpacingResult.value()[2];
    geometry.reconstructionVolumePixelSpacing[1] = reconstrucionPixelSpacingResult.value()[1];
    geometry.reconstructionVolumePixelSpacing[2] = reconstrucionPixelSpacingResult.value()[0];

    auto centerXmlElement = gridXmlElement.firstChildElement( "center" );
    if( centerXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError );
    }
    auto reconstrucionCenterPositionResult = QStringToPositionIn3DWorld( centerXmlElement.firstChild().nodeValue() );
    if( reconstrucionCenterPositionResult.has_error() )
    {
        qCritical() << QString::fromStdString( reconstrucionCenterPositionResult.error().message() );
        return make_error_code( TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError );
    }
    geometry.reconstructionVolumeCenterPositionInWorld = reconstrucionCenterPositionResult.value();

    auto resolutionXmlElement = gridXmlElement.firstChildElement( "resolution" );
    if( resolutionXmlElement.isNull() )
    {
        return make_error_code( TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError );
    }
    auto reconstructionVolumeSizeResult = QStringToSize3D( resolutionXmlElement.firstChild().nodeValue() );
    if( reconstructionVolumeSizeResult.has_error() )
    {
        qCritical() << QString::fromStdString( reconstructionVolumeSizeResult.error().message() );
        return make_error_code( TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError );
    }
    // z an x are inverted
    geometry.reconstructionVolumeSizeInPixels[0] = reconstructionVolumeSizeResult.value()[2];
    geometry.reconstructionVolumeSizeInPixels[1] = reconstructionVolumeSizeResult.value()[1];
    geometry.reconstructionVolumeSizeInPixels[2] = reconstructionVolumeSizeResult.value()[0];

    auto globalvolumeIndex = 0;
    for( auto zIndex = 0; zIndex < geometry.reconstructionVolumeSizeInPixels[2]; zIndex++ )
    {
        for( auto yIndex = 0; yIndex < geometry.reconstructionVolumeSizeInPixels[1]; yIndex++ )
        {
            for( auto xIndex = 0; xIndex < geometry.reconstructionVolumeSizeInPixels[0]; xIndex++ )
            {
                geometry.volumeIndices.push_back( globalvolumeIndex );
                globalvolumeIndex++;
            }
        }
    }

    geometry.reconstructionVolumeSizeInWorld[0] = static_cast<float>( geometry.reconstructionVolumeSizeInPixels[0] ) * geometry.reconstructionVolumePixelSpacing[0];
    geometry.reconstructionVolumeSizeInWorld[1] = static_cast<float>( geometry.reconstructionVolumeSizeInPixels[1] ) * geometry.reconstructionVolumePixelSpacing[1];
    geometry.reconstructionVolumeSizeInWorld[2] = static_cast<float>( geometry.reconstructionVolumeSizeInPixels[2] ) * geometry.reconstructionVolumePixelSpacing[2];

    geometry.volumeBLD[0] = geometry.reconstructionVolumeCenterPositionInWorld[0] - 0.5 * geometry.reconstructionVolumeSizeInWorld[0];
    geometry.volumeBLD[1] = geometry.reconstructionVolumeCenterPositionInWorld[1] - 0.5 * geometry.reconstructionVolumeSizeInWorld[1];
    geometry.volumeBLD[2] = geometry.reconstructionVolumeCenterPositionInWorld[2] - 0.5 * geometry.reconstructionVolumeSizeInWorld[2];

    geometry.volumeTRF[0] = geometry.volumeBLD[0] + geometry.reconstructionVolumeSizeInWorld[0];
    geometry.volumeTRF[1] = geometry.volumeBLD[1] + geometry.reconstructionVolumeSizeInWorld[0];
    geometry.volumeTRF[2] = geometry.volumeBLD[2] + geometry.reconstructionVolumeSizeInWorld[0];


    geometry.reconstructionXs.resize( geometry.reconstructionVolumeSizeInPixels[0] );
    geometry.reconstructionXs[0] = geometry.volumeBLD[0];
    for( auto xIndex{ 1 }; xIndex < geometry.reconstructionVolumeSizeInPixels[0]; xIndex++ )
    {
        geometry.reconstructionXs[xIndex] = geometry.reconstructionXs[xIndex - 1] + geometry.reconstructionVolumePixelSpacing[0];
    }

    geometry.reconstructionYs.resize( geometry.reconstructionVolumeSizeInPixels[1] );
    geometry.reconstructionYs[0] = geometry.volumeBLD[1];
    for( auto yIndex{ 1 }; yIndex < geometry.reconstructionVolumeSizeInPixels[1]; yIndex++ )
    {
        geometry.reconstructionYs[yIndex] = geometry.reconstructionYs[yIndex - 1] + geometry.reconstructionVolumePixelSpacing[1];
    }

    geometry.reconstructionZs.resize( geometry.reconstructionVolumeSizeInPixels[2] );
    geometry.reconstructionZs[0] = geometry.volumeBLD[2];
    for( auto zIndex{ 1 }; zIndex < geometry.reconstructionVolumeSizeInPixels[2]; zIndex++ )
    {
        geometry.reconstructionZs[zIndex] = geometry.reconstructionZs[zIndex - 1] + geometry.reconstructionVolumePixelSpacing[2];
    }


    return geometry;
}