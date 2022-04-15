#include "modules/geometry/RadonMatrixErrorCode.h"

#include "commons/TomoErrorCondition.h"

#include <cassert>

namespace    // anonymous namespace
{
//! Define a custom error code category derived from std::error_category
class RadonMatrixErrorCodeCategory : public std::error_category
{
public:
    //! Declare a global function returning a static instance of the custom category
    //! Note: the uniqueness of categories is guaranteed by comparing addresses, so it is achieved with a static object
    //! Note: in C++11 initialisation of static local variables is guaranteed to be thread-safe
    static RadonMatrixErrorCodeCategory & RadonMatrixErrorCodeCategory_instance();

    //! Return a short descriptive name for the category
    const char * name() const noexcept final;
    //! Return what each enum means in text
    std::string message( int p_condition ) const final;
    //! Allow generic ADAM error conditions to be compared to the specific errors above
    std::error_condition default_error_condition( int p_condition ) const noexcept final;
};
}    // end of anonymous namespace

RadonMatrixErrorCodeCategory & RadonMatrixErrorCodeCategory::RadonMatrixErrorCodeCategory_instance()
{
    static RadonMatrixErrorCodeCategory instance;
    return instance;
}

const char * RadonMatrixErrorCodeCategory::name() const noexcept
{
    return "RadonMatrixError";
}

std::string RadonMatrixErrorCodeCategory::message( int p_condition ) const
{
    switch( static_cast<RadonMatrixErrorCode>( p_condition ) )
    {
        case RadonMatrixErrorCode::InvalidGeometryError:
            return "Invalid geometry for Radon Matrix computation";
        case RadonMatrixErrorCode::InvalidRoi:
            return "Invalid Roi for Radon Matrix computation";
        case RadonMatrixErrorCode::FileLoadingProblem:
            return "Problem while Radon Matrix loading";
        case RadonMatrixErrorCode::FileSavingProblem:
            return "Problem while Radon Matrix saving";
    }

    assert( "Missing value for enum RadonMatrixErrorCode in RadonMatrixErrorCodeCategory::message" );
    return "Unknown RadonMatrix error";
}

std::error_condition RadonMatrixErrorCodeCategory::default_error_condition( int p_condition ) const noexcept
{
    switch( static_cast<RadonMatrixErrorCode>( p_condition ) )
    {
        case RadonMatrixErrorCode::InvalidGeometryError:
        case RadonMatrixErrorCode::InvalidRoi:
        case RadonMatrixErrorCode::FileSavingProblem:
        case RadonMatrixErrorCode::FileLoadingProblem:
            return make_error_condition( TomoErrorCondition::RadonMatrixError );
    }

    assert( "Missing value for enum RadonMatrixErrorCode in RadonMatrixErrorCodeCategory::default_error_condition" );
    return std::error_condition( p_condition, *this );    // I have no mapping for this code
}

//! Overload the global make_error_code() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_code make_error_code( RadonMatrixErrorCode p_error )
{
    return { static_cast<int>( p_error ), RadonMatrixErrorCodeCategory::RadonMatrixErrorCodeCategory_instance() };
}
