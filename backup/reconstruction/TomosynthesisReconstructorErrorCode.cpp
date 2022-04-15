#include "modules/reconstruction/TomosynthesisReconstructorErrorCode.h"

#include <cassert>

namespace    // anonymous namespace
{
//! Define a custom error code category derived from std::error_category
class TomosynthesisReconstructorErrorCodeCategory : public std::error_category
{
public:
    //! Declare a global function returning a static instance of the custom category
    //! Note: the uniqueness of categories is guaranteed by comparing addresses, so it is achieved with a static object
    //! Note: in C++11 initialisation of static local variables is guaranteed to be thread-safe
    static TomosynthesisReconstructorErrorCodeCategory & TomosynthesisReconstructorErrorCodeCategory_instance();

    //! Return a short descriptive name for the category
    const char * name() const noexcept final;
    //! Return what each enum means in text
    std::string message( int p_condition ) const final;
    //! Allow generic ADAM error conditions to be compared to the specific errors above
    std::error_condition default_error_condition( int p_condition ) const noexcept final;
};
}    // end of anonymous namespace

TomosynthesisReconstructorErrorCodeCategory & TomosynthesisReconstructorErrorCodeCategory::TomosynthesisReconstructorErrorCodeCategory_instance()
{
    static TomosynthesisReconstructorErrorCodeCategory instance;
    return instance;
}

const char * TomosynthesisReconstructorErrorCodeCategory::name() const noexcept
{
    return "TomosynthesisReconstructorError";
}

std::string TomosynthesisReconstructorErrorCodeCategory::message( int p_condition ) const
{
    switch( static_cast<TomosynthesisReconstructorErrorCode>( p_condition ) )
    {
        case TomosynthesisReconstructorErrorCode::MissingProjector:
            return "No projector defined in reconstructor methpd";
        case TomosynthesisReconstructorErrorCode::ProjectionsDataFileReadingProblem:
            return "DICOM file of projections data reading failed";
        case TomosynthesisReconstructorErrorCode::InvalidGeometryError:
            return "Geometry error";
        case TomosynthesisReconstructorErrorCode::GeometryInconsistency:
            return "Some inconsistencies in geometry lead to reconstruction impossibility";
        case TomosynthesisReconstructorErrorCode::BadAlgorithm:
            return "Bad reconstruction algorithm";
    }

    assert( "Missing value for enum TomoGeometryErrorCode in TomoGeometryErrorCodeCategory::message" );
    return "Unknown TomoGeometry error";
}

std::error_condition TomosynthesisReconstructorErrorCodeCategory::default_error_condition( int p_condition ) const noexcept
{
    switch( static_cast<TomosynthesisReconstructorErrorCode>( p_condition ) )
    {
        case TomosynthesisReconstructorErrorCode::InvalidGeometryError:
        case TomosynthesisReconstructorErrorCode::MissingProjector:
        case TomosynthesisReconstructorErrorCode::ProjectionsDataFileReadingProblem:
        case TomosynthesisReconstructorErrorCode::GeometryInconsistency:
        case TomosynthesisReconstructorErrorCode::BadAlgorithm:
            return make_error_condition( TomoErrorCondition::TomosynthesisReconstructorError );
    }

    assert( "Missing value for enum TomosynthesisReconstructorErrorCode in TomosynthesisReconstructorErrorCodeCategory::default_error_condition" );
    return std::error_condition( p_condition, *this );    // I have no mapping for this code
}

//! Overload the global make_error_code() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_code make_error_code( TomosynthesisReconstructorErrorCode p_error )
{
    return { static_cast<int>( p_error ), TomosynthesisReconstructorErrorCodeCategory::TomosynthesisReconstructorErrorCodeCategory_instance() };
}
