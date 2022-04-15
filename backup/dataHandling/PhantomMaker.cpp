#include "modules/dataHandling/PhantomMaker.h"

#include <QDebug>

#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>


ImageDataPtr PhantomMaker::GetPhantom() const
{
    // todo: add some checks

    auto indices = *( m_volume->GetVoxelsIndicesPtr() );
    auto voxelsGridPositions = *( m_volume->GetGridPositionsPtr() );
    std::vector<float> voxelsDensities( indices.size() );
    auto phantomFiller = [this, &voxelsGridPositions, &voxelsDensities]( int p_index ) {
        voxelsDensities[p_index] = m_backgroundDensity;
        for( const auto & paveAndDensity : m_paves )
        {
            if( paveAndDensity.pave.Contains( voxelsGridPositions[p_index] ) )
            {
                voxelsDensities[p_index] += paveAndDensity.density;
            }
        }
        for( const auto & sphereAndDensity : m_spheres )
        {
            if( sphereAndDensity.sphere.Contains( voxelsGridPositions[p_index] ) )
            {
                voxelsDensities[p_index] += sphereAndDensity.density;
            }
        }
    };


    std::for_each( std::execution::par_unseq, indices.cbegin(), indices.cend(), phantomFiller );

    auto size = m_volume->GetSize3D();

    auto voxelSpacing = m_volume->GetVoxelSpacing();
    auto outputVtkImage = ImageDataPtr::New();
    outputVtkImage->SetDimensions( size.x, size.y, size.z );
    outputVtkImage->SetSpacing( voxelSpacing.x, voxelSpacing.y, voxelSpacing.z );

    auto byteCount = voxelsDensities.size() * sizeof( float );
    outputVtkImage->AllocateScalars( VTK_FLOAT, 1 );
    // copy the internal cv::Mat data into the vtkImageData pointer
    std::memcpy( outputVtkImage->GetScalarPointer(), voxelsDensities.data(), byteCount );

    outputVtkImage->Modified();

    return outputVtkImage;
}
