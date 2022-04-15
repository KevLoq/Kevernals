#include "modules/projection/ProjectorErrorCode.h"

#include <cassert>

namespace    // anonymous namespace
{
//! Define a custom error code category derived from std::error_category
class ProjectorErrorCodeCategory : public std::error_category
{
public:
    //! Declare a global function returning a static instance of the custom category
    //! Note: the uniqueness of categories is guaranteed by comparing addresses, so it is achieved with a static object
    //! Note: in C++11 initialisation of static local variables is guaranteed to be thread-safe
    static ProjectorErrorCodeCategory & ProjectorErrorCodeCategory_instance();

    //! Return a short descriptive name for the category
    const char * name() const noexcept final;
    //! Return what each enum means in text
    std::string message( int p_condition ) const final;
    //! Allow generic ADAM error conditions to be compared to the specific errors above
    std::error_condition default_error_condition( int p_condition ) const noexcept final;
};
}    // end of anonymous namespace

ProjectorErrorCodeCategory & ProjectorErrorCodeCategory::ProjectorErrorCodeCategory_instance()
{
    static ProjectorErrorCodeCategory instance;
    return instance;
}

const char * ProjectorErrorCodeCategory::name() const noexcept
{
    return "ProjectorError";
}

std::string ProjectorErrorCodeCategory::message( int p_condition ) const
{
    switch( static_cast<ProjectorErrorCode>( p_condition ) )
    {
        case ProjectorErrorCode::RadonMatrixComputation:
            return "Radon matrix computation failed";
        case ProjectorErrorCode::ProjectionsDataFileReadingProblem:
            return "DICOM file of projections data reading failed";
        case ProjectorErrorCode::InvalidGeometryError:
            return "Geometry error";
        case ProjectorErrorCode::GeometryInconsistency:
            return "Some inconsistencies in geometry lead to reconstruction impossibility";
        case ProjectorErrorCode::VolumeProblemForProjection:
            return "Volume to be projected is not correct";
        case ProjectorErrorCode::BadAlgorithm:
            return "Bad reconstruction algorithm";
    }

    assert( "Missing value for enum TomoGeometryErrorCode in TomoGeometryErrorCodeCategory::message" );
    return "Unknown TomoGeometry error";
}

std::error_condition ProjectorErrorCodeCategory::default_error_condition( int p_condition ) const noexcept
{
    switch( static_cast<ProjectorErrorCode>( p_condition ) )
    {
        case ProjectorErrorCode::InvalidGeometryError:
        case ProjectorErrorCode::RadonMatrixComputation:
        case ProjectorErrorCode::ProjectionsDataFileReadingProblem:
        case ProjectorErrorCode::GeometryInconsistency:
        case ProjectorErrorCode::VolumeProblemForProjection:
        case ProjectorErrorCode::BadAlgorithm:
            return make_error_condition( TomoErrorCondition::TomosynthesisReconstructorError );
    }

    assert( "Missing value for enum ProjectorErrorCode in ProjectorErrorCodeCategory::default_error_condition" );
    return std::error_condition( p_condition, *this );    // I have no mapping for this code
}

//! Overload the global make_error_code() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_code make_error_code( ProjectorErrorCode p_error )
{
    return { static_cast<int>( p_error ), ProjectorErrorCodeCategory::ProjectorErrorCodeCategory_instance() };
}
