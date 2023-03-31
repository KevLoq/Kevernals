#include "modules/dataHandling/DICOMReader.h"

#include "modules/dataHandling/DICOMReaderErrorCode.h"
#include "modules/geometry/TomoGeometry.h"

#include <vtkDICOMMetaData.h>
#include <vtkDICOMReader.h>
#include <vtkDICOMValue.h>
#include <vtkErrorCode.h>
#include <vtkExtractVOI.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include <filesystem>
#include <iostream>
#include <string>

DICOMReader::DICOMReader( TomoGeometry * p_tomoGeometry )
  : m_tomoGeometry( p_tomoGeometry )
{}


Result<ImageDataPtr> DICOMReader::Read( std::string const & p_dicomFilePath ) const
{
    if( !std::filesystem::exists( p_dicomFilePath ) )
    {
        std::cout << "Raw projections dicom file does not exist: " << p_dicomFilePath << std::endl;
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        std::cout << PrintErrorCode( errorCode ) << std::endl;
        return errorCode;
    }

    vtkNew<vtkDICOMReader> reader;
    if( !reader->CanReadFile( p_dicomFilePath.c_str() ) )
    {
        std::cout << "Cannot read file: " << p_dicomFilePath << std::endl;
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        std::cout << PrintErrorCode( errorCode ) << std::endl;
        return errorCode;
    }

    reader->SetFileName( p_dicomFilePath.c_str() );

    // Update the image data
    reader->Update();
    if( reader->GetErrorCode() )
    {
        std::cout << "Error VTK " << reader->GetErrorCode() << " reading file " << reader->GetFileName() << std::endl;
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        std::cout << PrintErrorCode( errorCode ) << std::endl;
        return errorCode;
    }    // Update the meta data
    reader->UpdateInformation();

    auto projectionData = reader->GetOutput();
    if( projectionData == nullptr )
    {
        std::cout << "Empty data pointer for raw projections " << reader->GetFileName() << std::endl;
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        std::cout << PrintErrorCode( errorCode ) << std::endl;
        return errorCode;
    }

    auto projectionDataExtent = projectionData->GetExtent();
    int voiExtent[6];
    auto roiBottomLeft = m_tomoGeometry->GetProjectionROIsBLPixelPositionOnDetector();
    auto roiSize = m_tomoGeometry->projectionsRoisSize();
    voiExtent[0] = roiBottomLeft.x;
    voiExtent[1] = roiBottomLeft.x + roiSize.x - 1;
    voiExtent[2] = roiBottomLeft.y;
    voiExtent[3] = roiBottomLeft.y + roiSize.y - 1;
    voiExtent[4] = projectionDataExtent[4];
    voiExtent[5] = projectionDataExtent[5];

    auto voiExtractor = vtkSmartPointer<vtkExtractVOI>::New();
    voiExtractor->SetVOI( voiExtent );
    voiExtractor->SetInputData( projectionData );
    voiExtractor->Update();

    return voiExtractor->GetOutput();
}


Result<ImageDataPtr> DICOMReader::ReadDirectory( std::string const & p_dicomFilesContainedDirPath, std::vector<int> const & p_indicesToRemove ) const
{
    if( !std::filesystem::exists( p_dicomFilesContainedDirPath ) )
    {
        std::cout << "Raw projections dicom files directory does not exist: " << p_dicomFilesContainedDirPath << std::endl;
        auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
        std::cout << PrintErrorCode( errorCode ) << std::endl;
        return errorCode;
    }

    std::vector<ImageDataPtrAndTimeStamp> images;
    for( auto const & entry : std::filesystem::directory_iterator( p_dicomFilesContainedDirPath ) )
    {
        auto extension = entry.path().extension();
        if( extension.generic_string() == ".dcm" || extension.generic_string() == ".DCM" )
        {
            vtkNew<vtkDICOMReader> reader;
            auto currentImagePath = entry.path().generic_string();

            if( !reader->CanReadFile( currentImagePath.c_str() ) )
            {
                std::cout << "Cannot read file: " << currentImagePath << std::endl;
                auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
                std::cout << PrintErrorCode( errorCode ) << std::endl;
                return errorCode;
            }

            reader->SetFileName( currentImagePath.c_str() );

            // Update the image data
            reader->Update();
            if( reader->GetErrorCode() )
            {
                std::cout << "Error VTK " << reader->GetErrorCode() << " reading file " << reader->GetFileName() << std::endl;
                auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
                std::cout << PrintErrorCode( errorCode ) << std::endl;
                return errorCode;
            }    // Update the meta data
            reader->UpdateInformation();

            vtkDICOMMetaData * metaData = reader->GetMetaData();
            vtkDICOMValue dicomValue;

            ImageDataPtrAndTimeStamp currentElement;

            if( metaData->HasAttribute( DC::PatientID ) )
            {
                dicomValue = metaData->Get( DC::AcquisitionTime );
                currentElement.acquisitionTime = dicomValue.GetUTF8String( 0 );
            }

            currentElement.image = reader->GetOutput();

            if( currentElement.image == nullptr )
            {
                std::cout << "Empty data pointer for raw projections " << reader->GetFileName() << std::endl;
                auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
                std::cout << PrintErrorCode( errorCode ) << std::endl;
                return errorCode;
            }
            currentElement.fileName = reader->GetFileName();
            images.push_back( currentElement );
        }
    }

    std::sort( std::begin( images ), std::end( images ), []( ImageDataPtrAndTimeStamp const & p_imageA, ImageDataPtrAndTimeStamp const & p_imageB ) -> bool {
        return p_imageA.acquisitionTime < p_imageB.acquisitionTime;
    } );

    if( !p_indicesToRemove.empty() )
    {
        auto indices = p_indicesToRemove;
        std::sort( std::begin( indices ), std::end( indices ), std::greater<int>() );

        std::cout << "DICOMREADER" << std::endl;
        for( auto index : indices )
        {
            std::cout << index << std::endl;
            images.erase( std::begin( images ) + index );
        }
    }
    auto imagesAppender = vtkSmartPointer<vtkImageAppend>::New();
    imagesAppender->SetAppendAxis( 2 );
    for( auto const & currentImage : images )
    {
        std::cout << "Ordered raw projections " << currentImage.fileName << std::endl;
        auto projectionData = currentImage.image;
        if( projectionData == nullptr )
        {
            std::cout << "Empty data pointer for raw projections " << currentImage.fileName << std::endl;
            auto errorCode = DICOMReaderErrorCode::DataFileReadingProblem;
            std::cout << PrintErrorCode( errorCode ) << std::endl;
            return errorCode;
        }

        auto projectionDataExtent = projectionData->GetExtent();
        int voiExtent[6];
        auto roiBottomLeft = m_tomoGeometry->GetProjectionROIsBLPixelPositionOnDetector();
        auto roiSize = m_tomoGeometry->projectionsRoisSize();
        voiExtent[0] = roiBottomLeft.x;
        voiExtent[1] = roiBottomLeft.x + roiSize.x - 1;
        voiExtent[2] = roiBottomLeft.y;
        voiExtent[3] = roiBottomLeft.y + roiSize.y - 1;
        voiExtent[4] = projectionDataExtent[4];
        voiExtent[5] = projectionDataExtent[5];

        auto voiExtractor = vtkSmartPointer<vtkExtractVOI>::New();
        voiExtractor->SetVOI( voiExtent );
        voiExtractor->SetInputData( projectionData );
        voiExtractor->Update();

        imagesAppender->AddInputConnection( voiExtractor->GetOutputPort() );
    }

    imagesAppender->Update();

    return imagesAppender->GetOutput();
}