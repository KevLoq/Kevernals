#include "commons/GlobalUtils.h"
#include "commons/HistogramMatchingTools.h"
#include "commons/PrintErrorCode.h"
#include "modules/dataHandling/DICOMReader.h"
#include "modules/dataHandling/PhantomMaker.h"
#include "modules/geometry/TomoGeometry.h"
#include "modules/reconstruction/Projector.h"
#include "modules/reconstruction/Reconstructors.h"

#include <vtkImageCast.h>
#include <vtkImageFlip.h>
#include <vtkImageLogarithmicScale.h>
#include <vtkImageShiftScale.h>
#include <vtkTIFFWriter.h>

#include <chrono>
#include <optional>
#include <ratio>    // for std::milli
#include <string>

#define writeTestImages_

int main( int argc, char * argv[] )
{
    (void)argc;
    (void)argv;

    // auto computeGeometry{ true };
    auto doComputePhantom{ true };
    auto doComputeRadonMatrix{ true };
    auto doPerformProjection{ true };

    // Tiphaine Collomb
    // std::string tomoDataDirName{ "2018-02-02T10-21-57" };
    // std::string rawProjectionDicomFileName{ "RF.2.16.840.1.113669.632.3.2565175392124002300714440073152805124199.dcm" };
    // std::string geometryXmlFileName{ "tomos_2018-02-02T10-21-57.xml" };

    // geometric phantom
    /*std::string tomoDataDirName = "2018-09-05T14-29-03";
    std::string rawProjectionDicomFileName = "RF.2.16.840.1.113669.632.3.400305504133405787740011255270231557684.dcm";
    std::string geometryXmlFileName = "tomos_2018-09-05T14-29-03.xml";
    std::vector<int> imageIndicesToRemove;*/


    // flo phantom
    // std::string tomoDataDirName = "tomos_florian";
    // std::string rawProjectionDicomFileName = "dcm_data_files";
    // std::string geometryXmlFileName = "tomos_2022-06-13T16-14-05.xml";

    // trial PV2 03032023
    // std::string tomoDataDirName = "PV2_03032023";
    // std::string rawProjectionDicomFileName = "dcm_data_files";
    // std::string geometryXmlFileName = "tomos_2023-03-03T16-38-54.xml";

    // trial PV2 17032023
    /*std::string tomoDataDirName = "PV2_17032023";
    std::string rawProjectionDicomFileName = "dcm_data_files";
    std::string geometryXmlFileName = "tomos_2023-03-17T09-04-17.xml";*/
    // std::vector<int> xmlDataIndicesToRemove = { 0, 1, 25, 26 };
    // std::vector<int> imageIndicesToRemove = { 23, 24, 25 };
    //
    // trial PV2 31032023
    std::string tomoDataDirName = "PV2_31032023_001";
    std::string rawProjectionDicomFileName = "dcm_data_files";
    std::string geometryXmlFileName = "tomos_2023-03-31T12-15-50.xml";
    std::vector<int> xmlDataIndicesToRemove = { 0, 37, 38, 39, 40 };
    std::vector<int> imageIndicesToRemove{ 36, 37, 38, 39 };

    const std::string dataDirPath = "C:/Users/loquin.k/OneDrive - DMS/ADAM/Tomosynthesis/data/" + tomoDataDirName + "/";
    const std::string resultDirPath = dataDirPath + "results/";

    const std::string geometryXmlFilePath = dataDirPath + geometryXmlFileName;
    auto tomoGeometry = std::make_unique<TomoGeometry>( geometryXmlFilePath );
    tomoGeometry->PerformCheatingAdaptationsForDemonstration( xmlDataIndicesToRemove );

    if( !tomoGeometry->IsValid() )
    {
        std::cout << "invalid geometry parsing" << std::endl;
        glob::WaitForKeyTyping();
        return 1;
    }

    std::cout << "geometry parsed" << std::endl;

    std::cout << *tomoGeometry << std::endl;

    std::cout << "data file reading started" << std::endl;

    const std::string dataFilePath = dataDirPath + rawProjectionDicomFileName;
    DICOMReader dcmReader( tomoGeometry.get() );
    auto dataImageFileResult = dcmReader.ReadDirectory( dataFilePath, imageIndicesToRemove );
    if( dataImageFileResult.has_error() )
    {
        std::cout << "dataImageFileResult has error" << std::endl;
        std::cout << dataImageFileResult.error().message() << std::endl;
        glob::WaitForKeyTyping();
        return 1;
    }


    if( dataImageFileResult.value() == nullptr )
    {
        std::cout << "dataImageFileResult is nullptr" << std::endl;
        glob::WaitForKeyTyping();
        return 1;
    }
    // auto tiffWriterData = vtkSmartPointer<vtkTIFFWriter>::New();
    // tiffWriterData->SetFileName( "../dataImage.tiff" );
    // tiffWriterData->SetInputData( dataImageFileResult.value() );
    // tiffWriterData->Write();


    auto imageCastFilter = vtkSmartPointer<vtkImageCast>::New();
    imageCastFilter->SetInputData( dataImageFileResult.value() );
    imageCastFilter->SetOutputScalarTypeToFloat();
    imageCastFilter->Update();

    std::cout << "data file reading performed" << std::endl;

    auto logarithmicScalerFilter = vtkSmartPointer<vtkImageLogarithmicScale>::New();
    logarithmicScalerFilter->SetInputConnection( imageCastFilter->GetOutputPort() );
    logarithmicScalerFilter->SetConstant( 1. );
    logarithmicScalerFilter->Update();
    auto projectionsImage = logarithmicScalerFilter->GetOutput();

    {
        auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterBackProj->SetFileName( ( resultDirPath + "projectionImages.tiff" ).c_str() );
        tiffWriterBackProj->SetInputData( projectionsImage );
        tiffWriterBackProj->Write();
    }

    std::cout << "projection logarithm transformation performed" << std::endl;

    if( false )
    {
        auto reconstructionResult = recons::BackProjection( tomoGeometry.get(), projectionsImage );
        if( reconstructionResult.has_error() )
        {
            std::cout << PrintErrorCode( reconstructionResult.error() );
            glob::WaitForKeyTyping();
            return 1;
        }
        auto resultingVolume = reconstructionResult.value();
        auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterBackProj->SetFileName( ( resultDirPath + "backProjectionreconstructedImage.tiff" ).c_str() );
        tiffWriterBackProj->SetInputData( resultingVolume );
        tiffWriterBackProj->Write();
        std::cout << "backprojection reconstruction performed" << std::endl;
        std::cout << " ---  DONE  ---  ;)" << std::endl;
    }
    if( false )
    {
        auto reconstructionResult = recons::ShiftAndAdd( tomoGeometry.get(), projectionsImage, resultDirPath );
        if( reconstructionResult.has_error() )
        {
            std::cout << PrintErrorCode( reconstructionResult.error() );
            glob::WaitForKeyTyping();
            return 1;
        }
        auto resultingVolume = reconstructionResult.value();

        auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterBackProj->SetFileName( ( resultDirPath + "shiftAndAddReconstructedImage.tiff" ).c_str() );
        tiffWriterBackProj->SetInputData( resultingVolume );
        tiffWriterBackProj->Write();
        std::cout << "Shift and Add reconstruction performed" << std::endl;
        std::cout << " ---  DONE  ---  ;)" << std::endl;
    }
    if( true )
    {
        auto premierBPResult = recons::BackProjection( tomoGeometry.get(), projectionsImage );
        if( premierBPResult.has_error() )
        {
            std::cout << PrintErrorCode( premierBPResult.error() );
            glob::WaitForKeyTyping();
            return 1;
        }
        auto reconstructionResult = recons::ART( tomoGeometry.get(), projectionsImage, 5, 0.5F, premierBPResult.value(), resultDirPath );
        if( reconstructionResult.has_error() )
        {
            std::cout << PrintErrorCode( reconstructionResult.error() );
            glob::WaitForKeyTyping();
            return 1;
        }
        auto resultingVolume = reconstructionResult.value();

        auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterBackProj->SetFileName( ( resultDirPath + "ARTReconstructedImage.tiff" ).c_str() );
        tiffWriterBackProj->SetInputData( resultingVolume );
        tiffWriterBackProj->Write();
        std::cout << "ART reconstruction performed" << std::endl;
        std::cout << " ---  DONE  ---  ;)" << std::endl;
    }
    if( false )
    {
        auto premierBPResult = recons::BackProjection( tomoGeometry.get(), projectionsImage );
        if( premierBPResult.has_error() )
        {
            std::cout << PrintErrorCode( premierBPResult.error() );
            glob::WaitForKeyTyping();
            return 1;
        }
        auto reconstructionResult = recons::MLEM( tomoGeometry.get(), projectionsImage, 5, 0.5F, premierBPResult.value(), resultDirPath );
        if( reconstructionResult.has_error() )
        {
            std::cout << PrintErrorCode( reconstructionResult.error() );
            glob::WaitForKeyTyping();
            return 1;
        }
        auto resultingVolume = reconstructionResult.value();

        auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterBackProj->SetFileName( ( resultDirPath + "MLEMReconstructedImage.tiff" ).c_str() );
        tiffWriterBackProj->SetInputData( resultingVolume );
        tiffWriterBackProj->Write();
        std::cout << "MLEM reconstruction performed" << std::endl;
        std::cout << " ---  DONE  ---  ;)" << std::endl;
    }

    glob::WaitForKeyTyping();
    return 0;
}