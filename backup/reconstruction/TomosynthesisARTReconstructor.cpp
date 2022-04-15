#include "modules/reconstruction/TomosynthesisARTReconstructor.h"

#include "commons/GeometryUtils.h"
#include "commons/HistogramMatchingTools.h"
#include "commons/Maths.h"
#include "modules/reconstruction/TomosynthesisReconstructorErrorCode.h"

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

Result<ImageDataPtr> TomosynthesisARTReconstructor::PerformReconstruction()
{
    if( m_projector == nullptr )
    {
        auto errorCode = TomosynthesisReconstructorErrorCode::MissingProjector;
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return errorCode;
    }
#ifdef writeTestImages
    auto tiffWriter00 = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriter00->SetFileName( "../rawProjectionData.tiff" );
    tiffWriter00->SetInputData( m_projectionData );
    tiffWriter00->Write();
#endif
    auto reconstructedRawVolumeResult = m_projector->PerformBackprojection( m_projectionData );
    if( reconstructedRawVolumeResult.has_error() )
    {
        return reconstructedRawVolumeResult.error();
    }

#ifdef writeTestImages
    auto tiffWriter01 = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriter01->SetFileName( "../reconstructedRawVolume.tiff" );
    tiffWriter01->SetInputData( reconstructedRawVolumeResult.value() );
    tiffWriter01->Write();
#endif

    //rescaling for obtaining a complete dynamic range

    auto reconstructionSliceMaxValue = reconstructedRawVolumeResult.value()->GetScalarRange()[1];
    auto dynamicMax = static_cast<double>( USHRT_MAX - 1 );
    auto shiftScaleFilter = vtkSmartPointer<vtkImageShiftScale>::New();
    shiftScaleFilter->SetInputData( reconstructedRawVolumeResult.value() );
    shiftScaleFilter->SetOutputScalarTypeToUnsignedShort();
    shiftScaleFilter->SetScale( dynamicMax / reconstructionSliceMaxValue );
    shiftScaleFilter->ClampOverflowOn();
    shiftScaleFilter->Update();
    auto rescaledAndConvertedReconstructedImageSlice = shiftScaleFilter->GetOutput();

#ifdef writeTestImages
    auto tiffWriter03 = vtkSmartPointer<vtkTIFFWriter>::New();
    tiffWriter03->SetFileName( "../rescaledAndConvertedReconstructedImageSlice.tiff" );
    tiffWriter03->SetInputData( rescaledAndConvertedReconstructedImageSlice );
    tiffWriter03->Write();
#endif

    HistUniformization16Bit( rescaledAndConvertedReconstructedImageSlice );

    return rescaledAndConvertedReconstructedImageSlice;
}
