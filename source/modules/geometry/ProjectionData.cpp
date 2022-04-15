#include "modules/geometry/ProjectionData.h"


std::unique_ptr<ProjectionData> ProjectionData::Make( vtkSmartPointer<vtkImageData> p_projections, const std::vector<Position3D> & p_sourcesPositions )
{
    if( p_projections == nullptr )
    {
        return nullptr;
    }
    int dimensions[3];
    p_projections->GetDimensions( dimensions );
    if( p_sourcesPositions.empty() || dimensions[2] <= 0 || dimensions[2] != p_sourcesPositions.size() )
    {
        return nullptr;
    }
    return std::unique_ptr<ProjectionData>( new ProjectionData( p_projections, p_sourcesPositions ) );
}

ProjectionData::ProjectionData( vtkSmartPointer<vtkImageData> p_projections, const std::vector<Position3D> & p_sourcesPositions )
{
    m_projections = p_projections;
    const auto nbSources = p_sourcesPositions.size();
    m_sourcesPositions.resize( nbSources );
    for( size_t i = 0; i < nbSources; i++ )
    {
        m_sourcesPositions[i].x = p_sourcesPositions[i].x;
        m_sourcesPositions[i].y = p_sourcesPositions[i].y;
        m_sourcesPositions[i].z = p_sourcesPositions[i].z;
    }
}

void ProjectionData::GetDimensions( dim3 & p_dimension ) const
{
    int dimensions[3];
    m_projections->GetDimensions( dimensions );
    p_dimension.x = dimensions[0];
    p_dimension.y = dimensions[1];
    p_dimension.z = dimensions[2];
}


void ProjectionData::GetSpacing( float2 & p_spacing ) const
{
    double spacing[3];
    m_projections->GetSpacing( spacing );
    p_spacing.x = static_cast<float>( spacing[0] );
    p_spacing.y = static_cast<float>( spacing[1] );
}

// TODO revoir origin
void ProjectionData::GetOrigin( float3 & p_origin ) const
{
    double origin[3];
    m_projections->GetOrigin( origin );
    p_origin.x = static_cast<float>( origin[0] );
    p_origin.y = static_cast<float>( origin[1] );
    p_origin.z = static_cast<float>( origin[2] );
}