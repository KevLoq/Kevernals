#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/dataHandling/PhantomMaker.h"
#include "modules/geometry/TomoGeometry.h"
#include "modules/reconstruction/Projector.h"
#include "modules/reconstruction/ReconstructorsErrorCode.h"

#include <vtkExtractVOI.h>
#include <vtkImageAppend.h>
#include <vtkImageConstantPad.h>
#include <vtkImageTranslateExtent.h>
#include <vtkImageWeightedSum.h>
#include <vtkTIFFWriter.h>

namespace recons
{
Result<ImageDataPtr> ART( TomoGeometry * p_tomoGeometry,
                          ImageDataPtr p_projectionImages,
                          int p_iterationNumber,
                          float p_relaxationCoefficient,
                          std::optional<ImageDataPtr> p_initialVolume,
                          std::optional<std::string> p_outputDirectoryPath )
{
    ImageDataPtr resultingVolume;

    std::cout << "ART: initial volume retrieval" << std::endl;
    if( p_initialVolume.has_value() )
    {
        resultingVolume = p_initialVolume.value();
    }
    else
    {
        PhantomMaker phantomMaker( p_tomoGeometry->GetVolume(), 1.f );
        resultingVolume = phantomMaker.GetPhantom();
    }

    if( resultingVolume == nullptr )
    {
        std::cout << "ART: initial data is nullptr" << std::endl;
        return make_error_code( ReconstructorsErrorCode::ART );
    }

    auto verboseMode = p_outputDirectoryPath.has_value();

    if( verboseMode )
    {
        auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "initialVolume.tiff" ).c_str() );
        tiffWriterPhantom->SetInputData( resultingVolume );
        tiffWriterPhantom->Write();
    }


    auto projectionsImageDimensions = p_projectionImages->GetDimensions();
    auto projectionsImageBuffer = static_cast<float *>( p_projectionImages->GetScalarPointer() );

    auto resultingVolumeDimensions = resultingVolume->GetDimensions();
    auto resultingVolumeBuffer = static_cast<float *>( resultingVolume->GetScalarPointer() );


    Projector projector{ p_tomoGeometry };
    std::cout << "ART: reconstruction started" << std::endl;
    for( auto iteration{ 0 }; iteration < p_iterationNumber; iteration++ )
    {
        // step 1. projection
        auto currentProjection = projector.PerformProjection( resultingVolume );

        if( currentProjection == nullptr )
        {
            std::cout << "ART: current projection is nullptr" << std::endl;
            return make_error_code( ReconstructorsErrorCode::ART );
        }

        if( verboseMode )
        {
            auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
            tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "currentProjectionStep" + std::to_string( iteration ) + ".tiff" ).c_str() );
            tiffWriterPhantom->SetInputData( currentProjection );
            tiffWriterPhantom->Write();
        }

        // step 2. error computation
        auto currentDimensions = currentProjection->GetDimensions();
        auto currentBuffer = static_cast<float *>( currentProjection->GetScalarPointer() );
        if( currentDimensions[0] == projectionsImageDimensions[0] && currentDimensions[1] == projectionsImageDimensions[1] && currentDimensions[2] == projectionsImageDimensions[2] )
        {
            auto totalDim = currentDimensions[0] * currentDimensions[1] * currentDimensions[2];
            for( auto voxelIndex{ 0 }; voxelIndex < totalDim; voxelIndex++ )
            {
                currentBuffer[voxelIndex] = p_relaxationCoefficient * ( projectionsImageBuffer[voxelIndex] - currentBuffer[voxelIndex] );
            }
        }

        if( verboseMode )
        {
            auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
            tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "currentCorrectedProjectionStep" + std::to_string( iteration ) + ".tiff" ).c_str() );
            tiffWriterPhantom->SetInputData( currentProjection );
            tiffWriterPhantom->Write();
        }

        // step 3. error backProjection
        auto errorBackProjection = projector.PerformBackProjection( currentProjection );

        if( errorBackProjection == nullptr )
        {
            std::cout << "ART: current backprojection is nullptr" << std::endl;
            return make_error_code( ReconstructorsErrorCode::ART );
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


        if( verboseMode )
        {
            auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
            tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "reconstructionstep" + std::to_string( iteration ) + ".tiff" ).c_str() );
            tiffWriterPhantom->SetInputData( resultingVolume );
            tiffWriterPhantom->Write();
        }
    }
    return resultingVolume;
}

Result<ImageDataPtr> MLEM( TomoGeometry * p_tomoGeometry,
                           ImageDataPtr p_projectionImages,
                           int p_iterationNumber,
                           float p_relaxationCoefficient,
                           std::optional<ImageDataPtr> p_initialVolume,
                           std::optional<std::string> p_outputDirectoryPath )
{
    ImageDataPtr resultingVolume;

    std::cout << "MLEM: initial volume retrieval" << std::endl;
    if( p_initialVolume.has_value() )
    {
        resultingVolume = p_initialVolume.value();
    }
    else
    {
        PhantomMaker phantomMaker( p_tomoGeometry->GetVolume(), 1.f );
        resultingVolume = phantomMaker.GetPhantom();
    }

    if( resultingVolume == nullptr )
    {
        std::cout << "MLEM: initial data is nullptr" << std::endl;
        return make_error_code( ReconstructorsErrorCode::ART );
    }

    auto verboseMode = p_outputDirectoryPath.has_value();

    if( verboseMode )
    {
        auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
        tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "initialVolume.tiff" ).c_str() );
        tiffWriterPhantom->SetInputData( resultingVolume );
        tiffWriterPhantom->Write();
    }


    auto projectionsImageDimensions = p_projectionImages->GetDimensions();
    auto projectionsImageBuffer = static_cast<float *>( p_projectionImages->GetScalarPointer() );

    auto resultingVolumeDimensions = resultingVolume->GetDimensions();
    auto resultingVolumeBuffer = static_cast<float *>( resultingVolume->GetScalarPointer() );


    Projector projector{ p_tomoGeometry };
    std::cout << "MLEM: reconstruction started" << std::endl;
    for( auto iteration{ 0 }; iteration < p_iterationNumber; iteration++ )
    {
        // step 1. projection
        auto currentProjection = projector.PerformProjection( resultingVolume );

        if( currentProjection == nullptr )
        {
            std::cout << "MLEM: current projection is nullptr" << std::endl;
            return make_error_code( ReconstructorsErrorCode::ART );
        }

        if( verboseMode )
        {
            auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
            tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "currentProjectionStep" + std::to_string( iteration ) + ".tiff" ).c_str() );
            tiffWriterPhantom->SetInputData( currentProjection );
            tiffWriterPhantom->Write();
        }

        // step 2. error computation
        auto currentDimensions = currentProjection->GetDimensions();
        auto currentBuffer = static_cast<float *>( currentProjection->GetScalarPointer() );
        if( currentDimensions[0] == projectionsImageDimensions[0] && currentDimensions[1] == projectionsImageDimensions[1] && currentDimensions[2] == projectionsImageDimensions[2] )
        {
            auto totalDim = currentDimensions[0] * currentDimensions[1] * currentDimensions[2];
            for( auto voxelIndex{ 0 }; voxelIndex < totalDim; voxelIndex++ )
            {
                if( !AlmostEqualRelative( currentBuffer[voxelIndex], 0.F ) )
                {
                    currentBuffer[voxelIndex] = p_relaxationCoefficient * ( projectionsImageBuffer[voxelIndex] / currentBuffer[voxelIndex] );
                }
            }
        }

        if( verboseMode )
        {
            auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
            tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "currentCorrectedProjectionStep" + std::to_string( iteration ) + ".tiff" ).c_str() );
            tiffWriterPhantom->SetInputData( currentProjection );
            tiffWriterPhantom->Write();
        }

        // step 3. error backProjection
        auto errorBackProjection = projector.PerformBackProjection( currentProjection );

        if( errorBackProjection == nullptr )
        {
            std::cout << "MLEM: current backprojection is nullptr" << std::endl;
            return make_error_code( ReconstructorsErrorCode::ART );
        }
        // step 4. add backProjectedError
        auto errorBackProjectionDimensions = errorBackProjection->GetDimensions();
        auto errorBackProjectioncurrentBuffer = static_cast<float *>( errorBackProjection->GetScalarPointer() );
        if( errorBackProjectionDimensions[0] == resultingVolumeDimensions[0] && errorBackProjectionDimensions[1] == resultingVolumeDimensions[1] && errorBackProjectionDimensions[2] == resultingVolumeDimensions[2] )
        {
            auto totalDim = errorBackProjectionDimensions[0] * errorBackProjectionDimensions[1] * errorBackProjectionDimensions[2];
            for( auto voxelIndex{ 0 }; voxelIndex < totalDim; voxelIndex++ )
            {
                resultingVolumeBuffer[voxelIndex] *= errorBackProjectioncurrentBuffer[voxelIndex];
            }
        }

        if( verboseMode )
        {
            auto tiffWriterPhantom = vtkSmartPointer<vtkTIFFWriter>::New();
            tiffWriterPhantom->SetFileName( ( p_outputDirectoryPath.value() + "reconstructionstep" + std::to_string( iteration ) + ".tiff" ).c_str() );
            tiffWriterPhantom->SetInputData( resultingVolume );
            tiffWriterPhantom->Write();
        }
    }
    return resultingVolume;
}


Result<ImageDataPtr> BackProjection( TomoGeometry * p_tomoGeometry,
                                     ImageDataPtr p_projectionImages )
{
    Projector projector{ p_tomoGeometry };
    return projector.PerformBackProjection( p_projectionImages );
}

Result<ImageDataPtr> ShiftAndAdd( TomoGeometry * p_tomoGeometry,
                                  ImageDataPtr p_projectionImages,
                                  std::optional<std::string> p_outputDirectoryPath )
{
    ImageDataPtr resultingVolume;

    //  For notations used in the following code
    //  see article https://pubmed.ncbi.nlm.nih.gov/14579853/
    //  Digital x-ray tomosynthesis: current state of the art and clinical potential
    //  James T Dobbins 3rd 1, Devon J Godfrey

    auto nbReconstructedSlices = p_tomoGeometry->volumeSize3D().z;
    if( nbReconstructedSlices <= 1 )
    {
        std::cout << "ShiftAndAdd: required nb of slices in reconstruction " << std::to_string( nbReconstructedSlices ) << " <= 1" << std::endl;
        return make_error_code( ReconstructorsErrorCode::ShiftAndAdd );
    }
    auto nbProjections = p_tomoGeometry->nbProjections();
    if( nbProjections <= 1 )
    {
        std::cout << "ShiftAndAdd: required nb of projections " << std::to_string( nbProjections ) << " <= 1" << std::endl;
        return make_error_code( ReconstructorsErrorCode::ShiftAndAdd );
    }

    auto zF = p_tomoGeometry->fulcrum().z;

    auto verboseMode = p_outputDirectoryPath.has_value();
    if( verboseMode )
    {
        std::cout << "zF = " << std::to_string( zF ) << std::endl;
    }
    auto sid = p_tomoGeometry->sid();

    if( verboseMode )
    {
        std::cout << "sid = " << std::to_string( sid ) << std::endl;
    }

    if( AlmostEqualRelative( zF, sid ) )
    {
        std::cout << "ShiftAndAdd: Fulcrum and imager are at the same height: impossible" << std::endl;
        return make_error_code( ReconstructorsErrorCode::ShiftAndAdd );
    }


    auto mF = sid / ( sid - zF );

#ifdef verboseMode
    qInfo() << "mF = " << QString::number( mF );
#endif

    auto imagesAppender = vtkSmartPointer<vtkImageAppend>::New();
    imagesAppender->SetAppendAxis( 2 );
    auto yVolumePixelSpacing = p_tomoGeometry->volumeVoxelSpacing().y;
    for( auto reconstructionZ : p_tomoGeometry->volumeZs() )
    {
        if( AlmostEqualRelative( reconstructionZ, sid ) )
        {
            std::cout << "ShiftAndAdd: reconstruction slice and imager are at the same height: impossible" << std::endl;
            return make_error_code( ReconstructorsErrorCode::ShiftAndAdd );
        }
        auto mZ = sid / ( sid - reconstructionZ );
        auto translationYCoeff = ( mZ - mF );
        std::vector<int> translationYs;
        for( auto sourceY : p_tomoGeometry->sourcesYPositions() )
        {
            auto realWorldTranslationY = sourceY * translationYCoeff;
            translationYs.push_back( static_cast<int>( realWorldTranslationY / yVolumePixelSpacing ) );
        }
        auto maxTranslation = std::max_element( translationYs.cbegin(), translationYs.cend() );
        auto minTranslation = std::min_element( translationYs.cbegin(), translationYs.cend() );


        auto sumFilter = vtkSmartPointer<vtkImageWeightedSum>::New();


        auto imageConstantPad = vtkSmartPointer<vtkImageConstantPad>::New();
        imageConstantPad->SetInputData( p_projectionImages );
        auto projectionsExtent = p_projectionImages->GetExtent();

        // qDebug() << "demanded padded projections extent:    " << projectionsExtent[0] << " " << projectionsExtent[1]
        //          << " " << projectionsExtent[2] + minTranslation << " " << projectionsExtent[3] + maxTranslation
        //          << " " << projectionsExtent[4] << " " << projectionsExtent[5];
        imageConstantPad->SetOutputWholeExtent( projectionsExtent[0], projectionsExtent[1], projectionsExtent[2] + *minTranslation, projectionsExtent[3] + *maxTranslation, projectionsExtent[4], projectionsExtent[5] );
        imageConstantPad->UpdateWholeExtent();
        // auto paddedExtent = imageConstantPad->GetOutput()->GetExtent();
        // qDebug() << "global padded projections extent:    " << paddedExtent[0] << " " << paddedExtent[1]
        //          << " " << paddedExtent[2] << " " << paddedExtent[3]
        //          << " " << paddedExtent[4] << " " << paddedExtent[5];
        int projectionIndex = 0;
        for( auto translationY : translationYs )
        {
            auto voiExtractor = vtkSmartPointer<vtkExtractVOI>::New();
            // voiExtractor->SetVOI( voiExtentForSummation[0], voiExtentForSummation[1], voiExtentForSummation[2] + translationsInPixels[reconstructionSliceIndex], voiExtentForSummation[3] + translationsInPixels[reconstructionSliceIndex], reconstructionSliceIndex, reconstructionSliceIndex );
            voiExtractor->SetVOI( projectionsExtent[0], projectionsExtent[1], projectionsExtent[2] + translationY, projectionsExtent[3] + translationY, projectionIndex, projectionIndex );
            voiExtractor->SetInputConnection( imageConstantPad->GetOutputPort() );
            voiExtractor->UpdateWholeExtent();
            auto previousExtent = voiExtractor->GetOutput()->GetExtent();
            // qDebug() << "projection extraction extent:    " << previousExtent[0] << " " << previousExtent[1]
            //          << " " << previousExtent[2] << " " << previousExtent[3]
            //          << " " << previousExtent[4] << " " << previousExtent[5];


            auto translateExtent = vtkSmartPointer<vtkImageTranslateExtent>::New();
            translateExtent->SetTranslation( -previousExtent[0], -previousExtent[2], -previousExtent[4] );
            translateExtent->SetInputConnection( voiExtractor->GetOutputPort() );

            sumFilter->SetWeight( projectionIndex, 1. / static_cast<double>( nbProjections ) );
            sumFilter->AddInputConnection( translateExtent->GetOutputPort() );
            projectionIndex++;
        }

        imagesAppender->AddInputConnection( sumFilter->GetOutputPort() );
    }
    imagesAppender->Update();
    return imagesAppender->GetOutput();
}


};    // namespace recons