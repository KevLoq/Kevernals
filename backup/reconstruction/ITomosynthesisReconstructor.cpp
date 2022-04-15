#include "modules/reconstruction/ITomosynthesisReconstructor.h"

#include "commons/GeometryUtils.h"
#include "commons/Maths.h"
#include "modules/reconstruction/TomosynthesisARTReconstructor.h"
#include "modules/reconstruction/TomosynthesisFBPReconstructor.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <vtkDICOMReader.h>
#include <vtkExtractVOI.h>
#include <vtkImageCast.h>
#include <vtkImageLogarithmicScale.h>
#include <vtkNew.h>

#include <optional>

// set this pragma to verboseMode if you want to display more information at command prompt
// not used yet
#define _verboseMode
// set this pragma to writeTestImages if you want to save some temporary images for debugging purpose
#define _writeTestImages

#ifdef writeTestImages
#    include <vtkTIFFWriter.h>
#endif

std::unique_ptr<ITomosynthesisReconstructor> ITomosynthesisReconstructor::MakeReconstructor( TomosynthesisReconstructorMethod p_method )
{
    if( p_method == TomosynthesisReconstructorMethod::FBP )
    {
        return std::make_unique<TomosynthesisFBPReconstructor>();
        // return std::make_unique<NullTomosynthesisReconstructor>();
    }
    else if( p_method == TomosynthesisReconstructorMethod::ART )
    {
        return std::make_unique<TomosynthesisARTReconstructor>();
    }
    return std::make_unique<NullTomosynthesisReconstructor>();
}

ITomosynthesisReconstructor::ITomosynthesisReconstructor()
{
    m_projector = IProjector::MakeProjector( ProjectorMethod::ShiftAndAdd );
}

Result<void> ITomosynthesisReconstructor::SetProjectorMethod( ProjectorMethod p_method )
{
    if( m_projector == nullptr || p_method != m_projector->GetProjectorMethod() )
    {
        m_projector = IProjector::MakeProjector( p_method );
        return m_projector->SetGeometry( m_tomoGeometry );
    }
    return outcome::success();
}

Result<void> ITomosynthesisReconstructor::SetGeometry( const TomoGeometry & p_tomoGeometry )
{
    if( m_tomoGeometry != p_tomoGeometry )
    {
        m_tomoGeometry = p_tomoGeometry;
        if( m_projector == nullptr )
        {
            auto errorCode = TomosynthesisReconstructorErrorCode::MissingProjector;
            qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
            return errorCode;
        }
        return m_projector->SetGeometry( m_tomoGeometry );
    }
    return outcome::success();
}

ImageDataPtr ITomosynthesisReconstructor::GetProjectionData() const
{
    return m_projectionData;
}
void ITomosynthesisReconstructor::SetProjectionData( ImageDataPtr p_projectionData )
{
    m_projectionData = p_projectionData;
}

Result<void> ITomosynthesisReconstructor::LoadProjectionData( const QString & p_projectionDataFilePath, bool p_performLogarithmicTransform )
{
    QFileInfo rawProjectionDicomFilePathnfo{ p_projectionDataFilePath };
    if( !rawProjectionDicomFilePathnfo.exists() )
    {
        qCritical() << "Raw projections dicom file does not exist: " << p_projectionDataFilePath;
        auto errorCode = TomosynthesisReconstructorErrorCode::ProjectionsDataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    vtkNew<vtkDICOMReader> reader;
    if( !reader->CanReadFile( p_projectionDataFilePath.toStdString().c_str() ) )
    {
        qCritical() << "Cannot read file: " << p_projectionDataFilePath;
        auto errorCode = TomosynthesisReconstructorErrorCode::ProjectionsDataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    reader->SetFileName( p_projectionDataFilePath.toStdString().c_str() );

    // Update the image data
    reader->Update();
    if( reader->GetErrorCode() )
    {
        qCritical() << "Error VTK " << reader->GetErrorCode() << " reading file " << reader->GetFileName();
        auto errorCode = TomosynthesisReconstructorErrorCode::ProjectionsDataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }    // Update the meta data
    reader->UpdateInformation();

    auto projectionData = reader->GetOutput();
    if( projectionData == nullptr )
    {
        qCritical() << "Empty data pointer for raw projections " << reader->GetFileName();
        auto errorCode = TomosynthesisReconstructorErrorCode::ProjectionsDataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    auto projectionDataExtent = projectionData->GetExtent();
    int voiExtent[6];

    voiExtent[0] = std::min( std::min( m_tomoGeometry.projectionsRoi[0][0], m_tomoGeometry.projectionsRoi[1][0] ), std::min( m_tomoGeometry.projectionsRoi[2][0], m_tomoGeometry.projectionsRoi[3][0] ) );
    voiExtent[1] = std::max( std::max( m_tomoGeometry.projectionsRoi[0][0], m_tomoGeometry.projectionsRoi[1][0] ), std::max( m_tomoGeometry.projectionsRoi[2][0], m_tomoGeometry.projectionsRoi[3][0] ) );
    voiExtent[2] = std::min( std::min( m_tomoGeometry.projectionsRoi[0][1], m_tomoGeometry.projectionsRoi[1][1] ), std::min( m_tomoGeometry.projectionsRoi[2][1], m_tomoGeometry.projectionsRoi[3][1] ) );
    voiExtent[3] = std::max( std::max( m_tomoGeometry.projectionsRoi[0][1], m_tomoGeometry.projectionsRoi[1][1] ), std::max( m_tomoGeometry.projectionsRoi[2][1], m_tomoGeometry.projectionsRoi[3][1] ) );
    voiExtent[4] = projectionDataExtent[4];
    voiExtent[5] = projectionDataExtent[5];

    auto voiExtractor = vtkSmartPointer<vtkExtractVOI>::New();
    voiExtractor->SetVOI( voiExtent );
    voiExtractor->SetInputData( projectionData );
    voiExtractor->Update();
    m_projectionData = voiExtractor->GetOutput();

#ifdef writeTestImages
    auto tiffWriter = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriter->SetFileName( "../test16bitImage.tiff" );
    tiffWriter->SetInputData( imageDataPtrFull3D );
    tiffWriter->Write();
#endif

    if( p_performLogarithmicTransform )
    {
        auto floatConvertedFilter = vtkSmartPointer<vtkImageCast>::New();
        floatConvertedFilter->SetInputData( m_projectionData );
        floatConvertedFilter->SetOutputScalarTypeToFloat();
        floatConvertedFilter->Update();
        auto floatProjectedImage = floatConvertedFilter->GetOutput();

#ifdef writeTestImages
        if( projectionIndex == 10 || projectionIndex == 30 )
        {
            auto tiffWriter01 = vtkSmartPointer<vtkTIFFWriter>::New();
            QString sliceFilename = "../floatConverted" + QString::number( projectionIndex ) + ".tiff";
            tiffWriter01->SetFileName( sliceFilename.toStdString().c_str() );
            tiffWriter01->SetInputData( floatProjectedImage );
            tiffWriter01->Write();
        }
#endif

        auto logarithmicScalerFilter = vtkSmartPointer<vtkImageLogarithmicScale>::New();
        logarithmicScalerFilter->SetInputConnection( floatConvertedFilter->GetOutputPort() );
        logarithmicScalerFilter->SetConstant( 1. );
        logarithmicScalerFilter->Update();
        m_projectionData = logarithmicScalerFilter->GetOutput();
    }

    return outcome::success();
}
