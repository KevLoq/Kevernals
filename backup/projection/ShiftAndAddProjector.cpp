#include "modules/projection/ShiftAndAddProjector.h"

#include "commons/GeometryUtils.h"
#include "commons/Maths.h"
#include "modules/projection/ProjectorErrorCode.h"

#include <QDebug>
#include <vtkExtractVOI.h>
#include <vtkImageAppend.h>
#include <vtkImageConstantPad.h>
#include <vtkImageShiftScale.h>
#include <vtkImageTranslateExtent.h>
#include <vtkImageWeightedSum.h>
#include <vtkSmartPointer.h>

#include <algorithm>
#include <cmath>

// set this pragma to verboseMode if you want to display more information at command prompt
// not used yet
#define _verboseMode
// set this pragma to writeTestImages if you want to save some temporary images for debugging purpose
#define _writeTestImages

#ifdef writeTestImages
#    include < vtkTIFFWriter.h>
#endif


Result<void> ShiftAndAddProjector::SetGeometry( const TomoGeometry & p_tomoGeometry )
{
    if( m_tomoGeometry != p_tomoGeometry )
    {
        m_tomoGeometry = p_tomoGeometry;
        auto translationsComputationResult = this->ComputeTranslations();
        if( translationsComputationResult.has_error() )
        {
            return translationsComputationResult.error();
        }
    }
    return outcome::success();
}


Result<ImageDataPtr> ShiftAndAddProjector::PerformProjection( ImageDataPtr p_volume )
{
    auto nbReconstructedSlices = p_volume->GetDimensions()[2];
    if( nbReconstructedSlices <= 1 )
    {
        qCritical() << "Required nb of slices in reconstruction " << QString::number( nbReconstructedSlices ) << " <= 1 ";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    auto nbProjections = m_tomoGeometry.detectorCenterPositionsInWorld.size();
    if( nbProjections <= 1 )
    {
        qCritical() << "Required nb of projections " << QString::number( nbProjections ) << " <= 1 ";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    auto imagesAppender = vtkSmartPointer<vtkImageAppend>::New();
    imagesAppender->SetAppendAxis( 2 );

    if( nbProjections != m_reconstructionSlicesTranslationsForProjectionOperation.size() )
    {
        qCritical() << "Required nb of projections " << QString::number( nbReconstructedSlices ) << " problem";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    /*   int voiExtentForSummation[4];

    auto volumeCenterPixelX = p_volume->GetDimensions()[0] / 2;
    auto volumeCenterPixelY = p_volume->GetDimensions()[1] / 2;

    voiExtentForSummation[0] = volumeCenterPixelX - m_tomoGeometry.reconstructionVolumeSizeInPixels[0] / 2;
    voiExtentForSummation[1] = volumeCenterPixelX + m_tomoGeometry.reconstructionVolumeSizeInPixels[0] / 2;
    voiExtentForSummation[2] = volumeCenterPixelY - m_tomoGeometry.reconstructionVolumeSizeInPixels[1] / 2;
    voiExtentForSummation[3] = volumeCenterPixelY + m_tomoGeometry.reconstructionVolumeSizeInPixels[1] / 2;*/

    auto volumeExtent = p_volume->GetExtent();

    for( auto projectionIndex{ 0 }; projectionIndex < nbProjections; projectionIndex++ )
    {
        qInfo() << "projection index = " << QString::number( projectionIndex );

        auto translationsInPixels = m_reconstructionSlicesTranslationsForProjectionOperation[projectionIndex];
        if( nbReconstructedSlices != translationsInPixels.size() )
        {
            qCritical() << "Required nb of slices in reconstruction " << QString::number( nbReconstructedSlices ) << " problem";
            auto errorCode = ProjectorErrorCode::GeometryInconsistency;
            qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
            return errorCode;
        }

        auto minTranslation = *std::min_element( translationsInPixels.constBegin(), translationsInPixels.constEnd() );
        auto maxTranslation = *std::max_element( translationsInPixels.constBegin(), translationsInPixels.constEnd() );

        auto sumFilter = vtkSmartPointer<vtkImageWeightedSum>::New();

        auto imageConstantPad = vtkSmartPointer<vtkImageConstantPad>::New();
        imageConstantPad->SetInputData( p_volume );

        //qDebug() << "demanded padded projections extent:    " << volumeExtent[0] << " " << volumeExtent[1]
        //         << " " << volumeExtent[2] + minTranslation << " " << volumeExtent[3] + maxTranslation
        //         << " " << volumeExtent[4] << " " << volumeExtent[5];
        imageConstantPad->SetOutputWholeExtent( volumeExtent[0], volumeExtent[1], volumeExtent[2] + minTranslation, volumeExtent[3] + maxTranslation, volumeExtent[4], volumeExtent[5] );
        imageConstantPad->UpdateWholeExtent();
        //auto paddedExtent = imageConstantPad->GetOutput()->GetExtent();
        //qDebug() << "global padded projections extent:    " << paddedExtent[0] << " " << paddedExtent[1]
        //         << " " << paddedExtent[2] << " " << paddedExtent[3]
        //         << " " << paddedExtent[4] << " " << paddedExtent[5];

        for( auto reconstructionSliceIndex{ 0 }; reconstructionSliceIndex < nbReconstructedSlices; reconstructionSliceIndex++ )
        {
            auto voiExtractor = vtkSmartPointer<vtkExtractVOI>::New();
            // voiExtractor->SetVOI( voiExtentForSummation[0], voiExtentForSummation[1], voiExtentForSummation[2] + translationsInPixels[reconstructionSliceIndex], voiExtentForSummation[3] + translationsInPixels[reconstructionSliceIndex], reconstructionSliceIndex, reconstructionSliceIndex );
            voiExtractor->SetVOI( volumeExtent[0], volumeExtent[1], volumeExtent[2] + translationsInPixels[reconstructionSliceIndex], volumeExtent[3] + translationsInPixels[reconstructionSliceIndex], reconstructionSliceIndex, reconstructionSliceIndex );
            voiExtractor->SetInputConnection( imageConstantPad->GetOutputPort() );
            voiExtractor->UpdateWholeExtent();
            auto previousExtent = voiExtractor->GetOutput()->GetExtent();
            //qDebug() << "projection extraction extent:    " << previousExtent[0] << " " << previousExtent[1]
            //         << " " << previousExtent[2] << " " << previousExtent[3]
            //         << " " << previousExtent[4] << " " << previousExtent[5];

            auto translateExtent = vtkSmartPointer<vtkImageTranslateExtent>::New();
            translateExtent->SetTranslation( -previousExtent[0], -previousExtent[2], -previousExtent[4] );
            translateExtent->SetInputConnection( voiExtractor->GetOutputPort() );
            //translateExtent->UpdateWholeExtent();
            //auto translatepreviousExtent = translateExtent->GetOutput()->GetExtent();
            //qDebug() << "translated projection extraction extent    " << translatepreviousExtent[0] << " " << translatepreviousExtent[1]
            //         << " " << translatepreviousExtent[2] << " " << translatepreviousExtent[3]
            //         << " " << translatepreviousExtent[4] << " " << translatepreviousExtent[5];
            //translateExtent->Update();

            sumFilter->SetWeight( reconstructionSliceIndex, 1. / static_cast<double>( nbReconstructedSlices ) );
            sumFilter->AddInputConnection( translateExtent->GetOutputPort() );

            //sumFilter->UpdateWholeExtent();
            //auto extentsumFilteriAfterUpdateWholeExtent = sumFilter->GetOutput()->GetExtent();
            //qDebug() << "summed translated projection extent    " << extentsumFilteriAfterUpdateWholeExtent[0] << " " << extentsumFilteriAfterUpdateWholeExtent[1]
            //         << " " << extentsumFilteriAfterUpdateWholeExtent[2] << " " << extentsumFilteriAfterUpdateWholeExtent[3]
            //         << " " << extentsumFilteriAfterUpdateWholeExtent[4] << " " << extentsumFilteriAfterUpdateWholeExtent[5];
        }

        imagesAppender->AddInputConnection( sumFilter->GetOutputPort() );
    }
    //auto imageConstantPad = vtkSmartPointer<vtkImageConstantPad>::New();
    //imageConstantPad->SetInputConnection( imagesAppender->GetOutputPort() );
    //imageConstantPad->SetOutputWholeExtent( 0, , 0, volumeExtent[3] + maxTranslation + 1, 0, volumeExtent[5] );

    imagesAppender->Update();

    return imagesAppender->GetOutput();
}


Result<ImageDataPtr> ShiftAndAddProjector::PerformBackprojection( ImageDataPtr p_projections )
{
    auto projectionDataExtent = p_projections->GetExtent();
    auto nbProjections = projectionDataExtent[5] - projectionDataExtent[4] + 1;
    if( nbProjections <= 1 )
    {
        qCritical() << "Required nb of projections " << QString::number( nbProjections ) << " <= 1 ";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    auto imagesAppender = vtkSmartPointer<vtkImageAppend>::New();
    imagesAppender->SetAppendAxis( 2 );

    auto nbReconstructedSlices = m_tomoGeometry.reconstructionZs.size();
    if( nbReconstructedSlices <= 1 )
    {
        qCritical() << "Required nb of slices in reconstruction " << QString::number( nbReconstructedSlices ) << " <= 1 ";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    if( nbReconstructedSlices != m_projectionTranslationsForBackProjection.size() )
    {
        qCritical() << "Required nb of slices in reconstruction " << QString::number( nbReconstructedSlices ) << " problem";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
    auto projectionsExtent = p_projections->GetExtent();

    for( auto reconstructionSliceIndex{ 0 }; reconstructionSliceIndex < nbReconstructedSlices; reconstructionSliceIndex++ )
    {
        qInfo() << "reconstruction slice index = " << QString::number( reconstructionSliceIndex );

        auto translationsInPixels = m_projectionTranslationsForBackProjection[reconstructionSliceIndex];
        if( nbProjections != translationsInPixels.size() )
        {
            qCritical() << "Required nb of projections " << QString::number( nbProjections ) << " problem";
            auto errorCode = ProjectorErrorCode::GeometryInconsistency;
            qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
            return errorCode;
        }

        auto sumFilter = vtkSmartPointer<vtkImageWeightedSum>::New();

        auto minTranslation = *std::min_element( translationsInPixels.constBegin(), translationsInPixels.constEnd() );
        auto maxTranslation = *std::max_element( translationsInPixels.constBegin(), translationsInPixels.constEnd() );

        auto imageConstantPad = vtkSmartPointer<vtkImageConstantPad>::New();
        imageConstantPad->SetInputData( p_projections );

        //qDebug() << "demanded padded projections extent:    " << projectionsExtent[0] << " " << projectionsExtent[1]
        //         << " " << projectionsExtent[2] + minTranslation << " " << projectionsExtent[3] + maxTranslation
        //         << " " << projectionsExtent[4] << " " << projectionsExtent[5];
        imageConstantPad->SetOutputWholeExtent( projectionsExtent[0], projectionsExtent[1], projectionsExtent[2] + minTranslation, projectionsExtent[3] + maxTranslation, projectionsExtent[4], projectionsExtent[5] );
        imageConstantPad->UpdateWholeExtent();
        //auto paddedExtent = imageConstantPad->GetOutput()->GetExtent();
        //qDebug() << "global padded projections extent:    " << paddedExtent[0] << " " << paddedExtent[1]
        //         << " " << paddedExtent[2] << " " << paddedExtent[3]
        //         << " " << paddedExtent[4] << " " << paddedExtent[5];

        for( auto projectionIndex{ 0 }; projectionIndex < nbProjections; projectionIndex++ )
        {
            auto voiExtractor = vtkSmartPointer<vtkExtractVOI>::New();
            // voiExtractor->SetVOI( voiExtentForSummation[0], voiExtentForSummation[1], voiExtentForSummation[2] + translationsInPixels[reconstructionSliceIndex], voiExtentForSummation[3] + translationsInPixels[reconstructionSliceIndex], reconstructionSliceIndex, reconstructionSliceIndex );
            voiExtractor->SetVOI( projectionsExtent[0], projectionsExtent[1], projectionsExtent[2] + translationsInPixels[projectionIndex], projectionsExtent[3] + translationsInPixels[projectionIndex], projectionIndex, projectionIndex );
            voiExtractor->SetInputConnection( imageConstantPad->GetOutputPort() );
            voiExtractor->UpdateWholeExtent();
            auto previousExtent = voiExtractor->GetOutput()->GetExtent();
            //qDebug() << "projection extraction extent:    " << previousExtent[0] << " " << previousExtent[1]
            //         << " " << previousExtent[2] << " " << previousExtent[3]
            //         << " " << previousExtent[4] << " " << previousExtent[5];


            auto translateExtent = vtkSmartPointer<vtkImageTranslateExtent>::New();
            translateExtent->SetTranslation( -previousExtent[0], -previousExtent[2], -previousExtent[4] );
            translateExtent->SetInputConnection( voiExtractor->GetOutputPort() );

            sumFilter->SetWeight( projectionIndex, 1. / static_cast<double>( nbProjections ) );
            sumFilter->AddInputConnection( translateExtent->GetOutputPort() );
        }

        imagesAppender->AddInputConnection( sumFilter->GetOutputPort() );
    }
    imagesAppender->Update();
    return imagesAppender->GetOutput();
}


Result<void> ShiftAndAddProjector::ComputeTranslations()
{
    //  For notations used in the following code
    //  see article https://pubmed.ncbi.nlm.nih.gov/14579853/
    //  Digital x-ray tomosynthesis: current state of the art and clinical potential
    //  James T Dobbins 3rd 1, Devon J Godfrey

    auto nbReconstructedSlices = m_tomoGeometry.reconstructionVolumeSizeInPixels[2];
    if( nbReconstructedSlices <= 1 )
    {
        qCritical() << "Required nb of slices in reconstruction " << QString::number( nbReconstructedSlices ) << " <= 1 ";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }

    auto sourcePositions = m_tomoGeometry.xRaySourcesPositionsInWorld;
    auto detectorPositions = m_tomoGeometry.detectorCenterPositionsInWorld;
    auto nbProjections = sourcePositions.size();
    auto zF = m_tomoGeometry.rotationCenterInWorld[2];
#ifdef verboseMode
    qInfo() << "zF = " << QString::number( zF );
#endif

    auto D{ 0.f };
    // We take the average SID for D
    // But all the SIDs should be the same ones
    for( auto projectionIndex{ 0 }; projectionIndex < nbProjections; projectionIndex++ )
    {
        D += std::abs( sourcePositions[projectionIndex][2] - detectorPositions[projectionIndex][2] );
    }
    D /= static_cast<float>( nbProjections );

#ifdef verboseMode
    qInfo() << "D = " << QString::number( D );
#endif

    if( AlmostEqualRelative( zF, D ) )
    {
        qCritical() << "fulcrum and imager are at the same height: impossible";
        auto errorCode = ProjectorErrorCode::GeometryInconsistency;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }


    auto mF = D / ( D - zF );

#ifdef verboseMode
    qInfo() << "mF = " << QString::number( mF );
#endif

    m_projectionTranslationsForBackProjection.clear();
    m_reconstructionSlicesTranslationsForProjectionOperation.clear();
    m_projectionTranslationsForBackProjection.resize( nbReconstructedSlices );
    for( auto i{ 0 }; i < nbReconstructedSlices; i++ )
    {
        m_projectionTranslationsForBackProjection[i].clear();
        m_projectionTranslationsForBackProjection[i].resize( nbProjections );
    }
    m_reconstructionSlicesTranslationsForProjectionOperation.resize( nbProjections );

    for( auto i{ 0 }; i < nbProjections; i++ )
    {
        m_reconstructionSlicesTranslationsForProjectionOperation[i].clear();
        m_reconstructionSlicesTranslationsForProjectionOperation[i].resize( nbReconstructedSlices );
    }

    for( auto reconstructionSliceIndex{ 0 }; reconstructionSliceIndex < nbReconstructedSlices; reconstructionSliceIndex++ )
    {
        const auto z = m_tomoGeometry.reconstructionZs[reconstructionSliceIndex];
        if( AlmostEqualRelative( z, D ) )
        {
            qCritical() << "reconstruction slice and imager are at the same height: impossible";
            auto errorCode = ProjectorErrorCode::GeometryInconsistency;
            qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
            return errorCode;
        }
        auto mZ = D / ( D - z );
        auto translationYCoeff = ( mZ - mF );
        for( auto projectionIndex{ 0 }; projectionIndex < nbProjections; projectionIndex++ )
        {
            auto tempVal = sourcePositions[projectionIndex][1] /* we use y translations */ * translationYCoeff;
            m_projectionTranslationsForBackProjection[reconstructionSliceIndex][projectionIndex] = static_cast<int>( tempVal / m_tomoGeometry.reconstructionVolumePixelSpacing[1] );
            m_reconstructionSlicesTranslationsForProjectionOperation[projectionIndex][reconstructionSliceIndex] = -m_projectionTranslationsForBackProjection[reconstructionSliceIndex][projectionIndex];
        }
    }
    return outcome::success();
}
