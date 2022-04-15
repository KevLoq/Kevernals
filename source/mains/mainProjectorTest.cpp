#include "commons/HistogramMatchingTools.h" 
#include "modules/dataHandling/DICOMReader.h"
#include "modules/dataHandling/PhantomMaker.h" 
#include "modules/geometry/TomoGeometry.h"
#include "modules/reconstruction/Projector.h"

#include <QDebug>
#include <vtkImageCast.h>
#include <vtkImageFlip.h>
#include <vtkImageLogarithmicScale.h>
#include <vtkImageShiftScale.h>
#include <vtkTIFFWriter.h>

#include <chrono>
#include <cuda.h>
#include <cuda_runtime.h>
#include <optional>
#include <ratio>    // for std::milli

#define writeTestImages_

void waitForKeyTyping()
{
    do
    {
        std::cout << std::endl
                  << "Press any key to continue...";
    } while( std::cin.get() != '\n' );
}

int main( int argc, char * argv[] )
{
    Q_UNUSED( argc );
    Q_UNUSED( argv );


    //RadonMatrixElement currentElement;
    //qDebug() << "sizeof( RadonMatrixElement ) = " << sizeof( RadonMatrixElement );
    //qDebug() << "sizeof( std::vector<RadonMatrixElement> ) = " << sizeof( std::vector<RadonMatrixElement> );
    //qDebug() << "sizeof( RadonMatrixAggregate ) = " << sizeof( RadonMatrixAggregate );
    //qDebug() << "sizeof( std::vector<RadonMatrixAggregate> ) = " << sizeof( std::vector<RadonMatrixAggregate> );
    //auto testSize = 35000000;
    //auto projectionMatrix = std::make_unique<RadonMatrix>();
    //projectionMatrix->resize( testSize );
    //qDebug() << "sizeof( projectionMatrix ) = " << sizeof( projectionMatrix );
    //for( int i = 0; i < testSize; i++ )
    //{
    //    projectionMatrix->at( i ).resize( 120 );
    //    if( i % 1000000 == 0 )
    //    {
    //        qDebug() << i;
    //    }
    //}


    //auto computeGeometry{ true };
    auto doComputePhantom{ true };
    auto doComputeRadonMatrix{ true };
    auto doPerformProjection{ true };

    // Tiphaine Collomb
    QString tomoDataDirName{ "2018-02-02T10-21-57" };
    QString rawProjectionDicomFileName{ "RF.2.16.840.1.113669.632.3.2565175392124002300714440073152805124199.dcm" };
    QString geometryXmlFileName{ "tomos_2018-02-02T10-21-57.xml" };

    // geometric phantom
    //QString tomoDataDirName = "2018-09-05T14-29-03";
    //QString rawProjectionDicomFileName = "RF.2.16.840.1.113669.632.3.400305504133405787740011255270231557684.dcm";
    //QString geometryXmlFileName = "tomos_2018-09-05T14-29-03.xml";


    const QString dataDirPath = "C:/Users/loquin.k/OneDrive - DMS/ADAM/Tomosynthesis/data/" + tomoDataDirName + "/";
    const QString geometryXmlFilePath = dataDirPath + geometryXmlFileName;
    auto tomoGeometry = std::make_unique<TomoGeometry>( geometryXmlFilePath );

    if( !tomoGeometry->IsValid() )
    {
        qCritical() << "invalid geometry parsing";
        waitForKeyTyping();
        return 1;
    }

    qInfo() << "geometry parsed";

    qInfo() << "data file reading started";

    const QString dataFilePath = dataDirPath + rawProjectionDicomFileName;
    DICOMReader dcmReader( tomoGeometry.get() );
    auto dataImageFileResult = dcmReader.Read( dataFilePath );
    if( dataImageFileResult.has_error() )
    {
        qCritical() << "dataImageFileResult has error";
        qCritical() << QString::fromStdString( dataImageFileResult.error().message() );
        waitForKeyTyping();
        return 1;
    }

    if( dataImageFileResult.value() == nullptr )
    {
        qCritical() << "dataImageFileResult is nullptr";
        waitForKeyTyping();
        return 1;
    }
    auto tiffWriterData = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriterData->SetFileName( "../dataImage.tiff" );
    tiffWriterData->SetInputData( dataImageFileResult.value() );
    tiffWriterData->Write();


    auto imageCastFilter = vtkSmartPointer<vtkImageCast>::New();
    imageCastFilter->SetInputData( dataImageFileResult.value() );
    imageCastFilter->SetOutputScalarTypeToFloat();
    imageCastFilter->Update();

    qInfo() << "data file reading performed";

    auto logarithmicScalerFilter = vtkSmartPointer<vtkImageLogarithmicScale>::New();
    logarithmicScalerFilter->SetInputConnection( imageCastFilter->GetOutputPort() );
    logarithmicScalerFilter->SetConstant( 1. );
    logarithmicScalerFilter->Update();
    auto projectionsImage = logarithmicScalerFilter->GetOutput();

    qInfo() << "projection logarithm transformation performed";

    if( true )
    {
        qInfo() << "phantom computation started";


        auto nbIterations{ 20 };

        auto relaxationCoefficient{ 0.5f };

        ImageDataPtr resultingVolume;
        PhantomMaker phantomMaker( tomoGeometry->GetVolume(), 20.f );
        /*Pave paveToAdd( Position3D( 0.F, 0.F, 0.F ), WSize3D( 20.F, 40.F, 30.F ) );
        phantomMaker.AddPave( paveToAdd, 100.f );*/
        //Sphere sphereToAdd( Position3D( 60.f, -100.f, 150.f ), 15.f );
        //phantomMaker.AddSphere( sphereToAdd, 300.f );
        resultingVolume = phantomMaker.GetPhantom();

        if( resultingVolume == nullptr )
        {
            qCritical() << "initial data is nullptr";
            waitForKeyTyping();
            return 1;
        }
        auto tiffWriterGlobal = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterGlobal->SetFileName( "../phantom.tiff" );
        tiffWriterGlobal->SetInputData( resultingVolume );
        tiffWriterGlobal->Write();

        qInfo() << "phantom computation performed";

        qInfo() << "reconstruction started";

        Projector projector{ tomoGeometry.get() };


        auto projectionsImageDimensions = projectionsImage->GetDimensions();
        auto projectionsImageBuffer = static_cast<float *>( projectionsImage->GetScalarPointer() );

        auto resultingVolumeDimensions = resultingVolume->GetDimensions();
        auto resultingVolumeBuffer = static_cast<float *>( resultingVolume->GetScalarPointer() );


        for( auto iteration{ 0 }; iteration < nbIterations; iteration++ )
        {
            // step 1. projection
            auto currentProjection = projector.PerformProjection( resultingVolume );

            if( currentProjection == nullptr )
            {
                qCritical() << "current projection is nullptr";
                waitForKeyTyping();
                return 1;
            }

            // step 2. error computation
            auto currentDimensions = currentProjection->GetDimensions();
            auto currentBuffer = static_cast<float *>( currentProjection->GetScalarPointer() );
            if( currentDimensions[0] == projectionsImageDimensions[0] && currentDimensions[1] == projectionsImageDimensions[1] && currentDimensions[2] == projectionsImageDimensions[2] )
            {
                auto totalDim = currentDimensions[0] * currentDimensions[1] * currentDimensions[2];
                for( auto voxelIndex{ 0 }; voxelIndex < totalDim; voxelIndex++ )
                {
                    currentBuffer[voxelIndex] = relaxationCoefficient * ( projectionsImageBuffer[voxelIndex] - currentBuffer[voxelIndex] );
                }
            }
            // step 3. error backProjection
            auto errorBackProjection = projector.PerformBackProjection( currentProjection );

            if( errorBackProjection == nullptr )
            {
                qCritical() << "current backprojection is nullptr";
                waitForKeyTyping();
                return 1;
            }
            // step 4. add backProjectedError
            auto errorBackProjectionDimensions = errorBackProjection->GetDimensions();
            auto errorBackProjectioncurrentBuffer = static_cast<float *>( errorBackProjection->GetScalarPointer() );
            if( errorBackProjectionDimensions[0] == resultingVolumeDimensions[0] && errorBackProjectionDimensions[1] == resultingVolumeDimensions[1] && errorBackProjectionDimensions[2] == resultingVolumeDimensions[2] )
            {
                auto totalDim = errorBackProjectionDimensions[0] * errorBackProjectionDimensions[1] * errorBackProjectionDimensions[2];
                for( auto voxelIndex{ 0 }; voxelIndex < totalDim; voxelIndex++ )
                {
                    resultingVolumeBuffer[voxelIndex] += errorBackProjectioncurrentBuffer[voxelIndex];
                }
            }

            auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
            tiffWriterBackProj->SetFileName( ( "../reconstructionstep" + std::to_string( iteration ) + ".tiff" ).c_str() );
            tiffWriterBackProj->SetInputData( resultingVolume );
            tiffWriterBackProj->Write();
        }

        qInfo() << "reconstruction performed";


        //auto projectedImageResult = projector.PerformProjection( resultingVolume );
        //if( projectedImageResult == nullptr )
        //{
        //    qCritical() << QString::fromStdString( "projection failed" );
        //    waitForKeyTyping();
        //    return 1;
        //}


        //auto tiffWriterProj = vtkSmartPointer<vtkTIFFWriter>::New();
        //tiffWriterProj->SetFileName( "../projectedImage.tiff" );
        //tiffWriterProj->SetInputData( projectedImageResult );
        //tiffWriterProj->Write();


        //auto backProjectedImageResult = projector.PerformBackProjection( projectedImageResult );
        //if( backProjectedImageResult == nullptr )
        //{
        //    qCritical() << QString::fromStdString( "back projection failed" );
        //    waitForKeyTyping();
        //    return 1;
        //}

        //auto tiffWriterProj2 = vtkSmartPointer<vtkTIFFWriter>::New();
        //tiffWriterProj2->SetFileName( "../backProjectedImage.tiff" );
        //tiffWriterProj2->SetInputData( backProjectedImageResult );
        //tiffWriterProj2->Write();
    }


    //for( auto iteration{ 0 }; iteration < nbIterations; iteration++ )
    //{
    //    // step 1. projection
    //    auto currentProjectionResult = projector.PerformProjection( resultingVolume );

    //    if( currentProjectionResult.has_error() )
    //    {
    //        qCritical() << QString::fromStdString( currentProjectionResult.error().message() );
    //        qtin.readLine();
    //        return 1;
    //    }

    //    // step 2. error computation
    //    auto currentProjection = currentProjectionResult.value();
    //    auto currentDimensions = currentProjection->GetDimensions();
    //    auto currentBuffer = static_cast<float *>( currentProjection->GetScalarPointer() );
    //    if( currentDimensions[0] == projectionsImageDimensions[0] && currentDimensions[1] == projectionsImageDimensions[1] && currentDimensions[2] == projectionsImageDimensions[2] )
    //    {
    //        auto totalDim = currentDimensions[0] * currentDimensions[1] * currentDimensions[2];
    //        for( auto voxelIndex{ 0 }; voxelIndex < totalDim; voxelIndex++ )
    //        {
    //            currentBuffer[voxelIndex] = relaxationCoefficient * ( projectionsImageBuffer[voxelIndex] - currentBuffer[voxelIndex] );
    //        }
    //    }
    //    // step 3. error backProjection
    //    auto errorBackProjectionResult = projector.PerformBackProjection( currentProjection );

    //    if( errorBackProjectionResult.has_error() )
    //    {
    //        qCritical() << QString::fromStdString( errorBackProjectionResult.error().message() );
    //        qtin.readLine();
    //        return 1;
    //    }
    //    // step 4. add backProjectedError
    //    auto errorBackProjection = errorBackProjectionResult.value();
    //    auto errorBackProjectionDimensions = errorBackProjection->GetDimensions();
    //    auto errorBackProjectioncurrentBuffer = static_cast<float *>( errorBackProjection->GetScalarPointer() );
    //    if( errorBackProjectionDimensions[0] == resultingVolumeDimensions[0] && errorBackProjectionDimensions[1] == resultingVolumeDimensions[1] && errorBackProjectionDimensions[2] == resultingVolumeDimensions[2] )
    //    {
    //        auto totalDim = errorBackProjectionDimensions[0] * errorBackProjectionDimensions[1] * errorBackProjectionDimensions[2];
    //        for( auto voxelIndex{ 0 }; voxelIndex < totalDim; voxelIndex++ )
    //        {
    //            resultingVolumeBuffer[voxelIndex] += errorBackProjectioncurrentBuffer[voxelIndex];
    //        }
    //    }

    //    auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
    //    tiffWriterBackProj->SetFileName( ( "../reconstructionstep" + std::to_string( iteration ) + ".tiff" ).c_str() );
    //    tiffWriterBackProj->SetInputData( resultingVolume );
    //    tiffWriterBackProj->Write();
    //}

    //qInfo() << "reconstruction performed";

    //qInfo() << "back projection started";
    //
    //auto backProjectedImageResult = projector.PerformBackProjection( imageCastFilter->GetOutput() );
    //if( backProjectedImageResult.has_error() )
    //{
    //    qCritical() << "backProjectedImageResult has error";
    //    qCritical() << QString::fromStdString( backProjectedImageResult.error().message() );
    //    qtin.readLine();
    //    return 1;
    //}

    //qInfo() << "back projection performed";

    //if( backProjectedImageResult.value() == nullptr )
    //{
    //    qCritical() << "backProjectedImage is nullptr";
    //    qtin.readLine();
    //    return 1;
    //}

    //auto nextFilterInput = resultingVolume;

    //auto tiffWriterBackProj00 = vtkSmartPointer<vtkTIFFWriter>::New();
    //tiffWriterBackProj00->SetFileName( "../backProjectedImage.tiff" );
    //tiffWriterBackProj00->SetInputData( nextFilterInput );
    //tiffWriterBackProj00->Write();


    //qInfo() << "flips started";

    //// inversions
    //auto rawflipfilter = vtkSmartPointer<vtkImageFlip>::New();
    //rawflipfilter->SetFilteredAxis( 2 );    // flip slides axis
    //rawflipfilter->SetInputData( nextFilterInput );
    //rawflipfilter->Update();
    //nextFilterInput = rawflipfilter->GetOutput();

    //auto rawflipfilter2 = vtkSmartPointer<vtkImageFlip>::New();
    //rawflipfilter2->SetFilteredAxis( 1 );    // flip horizontal axis
    //rawflipfilter2->SetInputData( nextFilterInput );
    //rawflipfilter2->Update();
    //nextFilterInput = rawflipfilter2->GetOutput();

    //qInfo() << "flips performed";

    //qInfo() << "histo uniformization started";
    ////rescaling for obtaining a complete dynamic range

    //auto reconstructionSliceMaxValue = nextFilterInput->GetScalarRange()[1];
    //auto dynamicMax = static_cast<double>( USHRT_MAX - 1 );
    //auto shiftScaleFilter = vtkSmartPointer<vtkImageShiftScale>::New();
    //shiftScaleFilter->SetInputData( nextFilterInput );
    //shiftScaleFilter->SetOutputScalarTypeToUnsignedShort();
    //shiftScaleFilter->SetScale( dynamicMax / reconstructionSliceMaxValue );
    //shiftScaleFilter->ClampOverflowOn();
    //shiftScaleFilter->Update();
    //auto rescaledAndConvertedReconstructedImage = shiftScaleFilter->GetOutput();

    //HistUniformization16Bit( rescaledAndConvertedReconstructedImage );
    //qInfo() << "histo uniformization performed";

    //auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
    //tiffWriterBackProj->SetFileName( "../backProjectedImageProcessed.tiff" );
    //tiffWriterBackProj->SetInputData( rescaledAndConvertedReconstructedImage );
    //tiffWriterBackProj->Write();


    /*

    ImageDataPtr phantom;
    qInfo() << "phantom computation";
    PhantomMaker phantomMaker( tomoGeometry->GetVolume() );
    Pave paveToAdd( Position3D( -25.f, -50.f, 207.f ), WSize3D( 20.f, 40.f, 30.f ) );
    Sphere sphereToAdd( Position3D( 60.f, -100.f, 220.f ), 15.f );
    phantomMaker.AddPave( paveToAdd, 100.f );
    phantomMaker.AddSphere( sphereToAdd, 300.f );
    phantom = phantomMaker.GetPhantom();

    if( phantom == nullptr )
    {
        qCritical() << "projection is nullptr";
        qtin.readLine();
        return 1;
    }
    auto tiffWriterGlobal = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriterGlobal->SetFileName( "../phantom.tiff" );
    tiffWriterGlobal->SetInputData( phantom );
    tiffWriterGlobal->Write();
    qInfo() << "phantom done";


    //RadonMatrixManager radonMatrixManager( tomoGeometry.get() );
    //auto matrixComputationResult = radonMatrixManager.ComputeProjectionMatrix( RadonMatrixManager::RadonMatrixComputationMethod::RayTracing );
    //if( matrixComputationResult.has_error() )
    //{
    //    qCritical() << QString::fromStdString( matrixComputationResult.error().message() );
    //    qtin.readLine();
    //    return 1;
    //}
    //qInfo() << "ComputeProjectionMatrix done";
    //auto projMatrixPtr = matrixComputationResult.value().get();

    Projector projector{ tomoGeometry.get() };

    qInfo() << "projection started";
    auto projectedImageResult = projector.PerformProjection( phantom );
    if( projectedImageResult.has_error() )
    {
        qCritical() << "projectedImageResult has error";
        qCritical() << QString::fromStdString( projectedImageResult.error().message() );
        qtin.readLine();
        return 1;
    }

    qInfo() << "projection performed";

    if( projectedImageResult.value() == nullptr )
    {
        qCritical() << "projectedImage is nullptr";
        qtin.readLine();
        return 1;
    }
    auto tiffWriterProj = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriterProj->SetFileName( "../projectedImage.tiff" );
    tiffWriterProj->SetInputData( projectedImageResult.value() );
    tiffWriterProj->Write();

    qInfo() << "back projection started";

    auto backProjectedImageResult = projector.PerformBackProjection( projectedImageResult.value() );
    if( backProjectedImageResult.has_error() )
    {
        qCritical() << "backProjectedImageResult has error";
        qCritical() << QString::fromStdString( backProjectedImageResult.error().message() );
        qtin.readLine();
        return 1;
    }

    qInfo() << "back projection performed";

    if( backProjectedImageResult.value() == nullptr )
    {
        qCritical() << "backProjectedImage is nullptr";
        qtin.readLine();
        return 1;
    }
    auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriterBackProj->SetFileName( "../backProjectedImage.tiff" );
    tiffWriterBackProj->SetInputData( backProjectedImageResult.value() );
    tiffWriterBackProj->Write();


    //auto backProjMatrixComputationResult = radonMatrixManager.ComputeItsBackProjectionMatrix( projMatrixPtr );
    //if( backProjMatrixComputationResult.has_error() )
    //{
    //    qCritical() << QString::fromStdString( backProjMatrixComputationResult.error().message() );
    //    qtin.readLine();
    //    return 1;
    //}
    //qInfo() << "ComputeBackProjectionMatrix done";

    //auto backProjMatrixPtr = backProjMatrixComputationResult.value().get();

    //auto backProjectedImageResult = projector.PerformBackprojection( backProjMatrixPtr, projectedImageResult.value() );
    //if( backProjectedImageResult.has_error() )
    //{
    //    qCritical() << "backProjectedImageResult has error";
    //    qCritical() << QString::fromStdString( backProjectedImageResult.error().message() );
    //    qtin.readLine();
    //    return 1;
    //}

    //qInfo() << "backprojection performed";
    //if( backProjectedImageResult.value() == nullptr )
    //{
    //    qCritical() << "projectedImage is nullptr";
    //    qtin.readLine();
    //    return 1;
    //}
    //auto tiffWriterBackProj = vtkSmartPointer<vtkTIFFWriter>::New();
    //tiffWriterBackProj->SetFileName( "../backProjectedImage.tiff" );
    //tiffWriterBackProj->SetInputData( backProjectedImageResult.value() );
    //tiffWriterBackProj->Write();
    
    */
    qInfo() << " ---  DONE  ---  ;)";
    waitForKeyTyping();
    return 0;
}
