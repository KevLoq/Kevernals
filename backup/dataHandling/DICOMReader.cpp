#include "modules/dataHandling/DICOMReader.h"

#include "modules/dataHandling/DICOMReaderErrorCode.h"
#include "modules/geometry/TomoGeometry.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <vtkDICOMReader.h>
#include <vtkExtractVOI.h>
#include <vtkImageCast.h>
#include <vtkNew.h>

DICOMReader::DICOMReader( TomoGeometry * p_tomoGeometry )
  : m_tomoGeometry( p_tomoGeometry )
{}


Result<ImageDataPtr> DICOMReader::Read( const QString & p_dicomFilePath ) const
{
    QFileInfo rawDicomFilePathInfo{ p_dicomFilePath };
    if( !rawDicomFilePathInfo.exists() )
    {
        qCritical() << "Raw projections dicom file does not exist: " << p_dicomFilePath;
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    vtkNew<vtkDICOMReader> reader;
    if( !reader->CanReadFile( p_dicomFilePath.toStdString().c_str() ) )
    {
        qCritical() << "Cannot read file: " << p_dicomFilePath;
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    reader->SetFileName( p_dicomFilePath.toStdString().c_str() );

    // Update the image data
    reader->Update();
    if( reader->GetErrorCode() )
    {
        qCritical() << "Error VTK " << reader->GetErrorCode() << " reading file " << reader->GetFileName();
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }    // Update the meta data
    reader->UpdateInformation();

    auto projectionData = reader->GetOutput();
    if( projectionData == nullptr )
    {
        qCritical() << "Empty data pointer for raw projections " << reader->GetFileName();
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    auto projectionDataExtent = projectionData->GetExtent();
    int voiExtent[6];
    const auto roiBottomLeft = m_tomoGeometry->GetProjectionROIsBLPixelPositionOnDetector();
    const auto roiTopRight = m_tomoGeometry->GetProjectionROIsBLPixelPositionOnDetector() + m_tomoGeometry->projectionsRoisSize();
    voiExtent[0] = roiBottomLeft.x;
    voiExtent[1] = roiTopRight.x - 1;
    voiExtent[2] = roiBottomLeft.y;
    voiExtent[3] = roiTopRight.y - 1;
    voiExtent[4] = projectionDataExtent[4];
    voiExtent[5] = projectionDataExtent[5];

    auto voiExtractor = vtkSmartPointer<vtkExtractVOI>::New();
    voiExtractor->SetVOI( voiExtent );
    voiExtractor->SetInputData( projectionData );
    voiExtractor->Update();

    return voiExtractor->GetOutput();
}
