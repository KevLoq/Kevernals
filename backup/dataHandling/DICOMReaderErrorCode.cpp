#include "modules/dataHandling/DICOMReaderErrorCode.h"

#include <cassert>

namespace    // anonymous namespace
{
//! Define a custom error code category derived from std::error_category
class DICOMReaderErrorCodeCategory : public std::error_category
{
public:
    //! Declare a global function returning a static instance of the custom category
    //! Note: the uniqueness of categories is guaranteed by comparing addresses, so it is achieved with a static object
    //! Note: in C++11 initialisation of static local variables is guaranteed to be thread-safe
    static DICOMReaderErrorCodeCategory & DICOMReaderErrorCodeCategory_instance();

    //! Return a short descriptive name for the category
    const char * name() const noexcept final;
    //! Return what each enum means in text
    std::string message( int p_condition ) const final;
    //! Allow generic ADAM error conditions to be compared to the specific errors above
    std::error_condition default_error_condition( int p_condition ) const noexcept final;
};
}    // end of anonymous namespace

DICOMReaderErrorCodeCategory & DICOMReaderErrorCodeCategory::DICOMReaderErrorCodeCategory_instance()
{
    static DICOMReaderErrorCodeCategory instance;
    return instance;
}

const char * DICOMReaderErrorCodeCategory::name() const noexcept
{
    return "DICOMReaderError";
}

std::string DICOMReaderErrorCodeCategory::message( int p_condition ) const
{
    switch( static_cast<DICOMReaderErrorCode>( p_condition ) )
    {
        case DICOMReaderErrorCode::DataFileReadingProblem:
            return "DICOM file reading failed";
    }

    assert( "Missing value for enum TomoGeometryErrorCode in TomoGeometryErrorCodeCategory::message" );
    return "Unknown TomoGeometry error";
}

std::error_condition DICOMReaderErrorCodeCategory::default_error_condition( int p_condition ) const noexcept
{
    switch( static_cast<DICOMReaderErrorCode>( p_condition ) )
    {
        case DICOMReaderErrorCode::DataFileReadingProblem:
            return make_error_condition( TomoErrorCondition::DICOMReadError );
    }

    assert( "Missing value for enum DICOMReaderErrorCode in DICOMReaderErrorCodeCategory::default_error_condition" );
    return std::error_condition( p_condition, *this );    // I have no mapping for this code
}

//! Overload the global make_error_code() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_code make_error_code( DICOMReaderErrorCode p_error )
{
    return { static_cast<int>( p_error ), DICOMReaderErrorCodeCategory::DICOMReaderErrorCodeCategory_instance() };
}
