#include "modules/altumira/AltumiraErrorCode.h"

#include "commons/TomoErrorCondition.h"

#include <cassert>

namespace    // anonymous namespace
{
//! Define a custom error code category derived from std::error_category
class AltumiraErrorCodeCategory : public std::error_category
{
public:
    //! Declare a global function returning a static instance of the custom category
    //! Note: the uniqueness of categories is guaranteed by comparing addresses, so it is achieved with a static object
    //! Note: in C++11 initialisation of static local variables is guaranteed to be thread-safe
    static AltumiraErrorCodeCategory & AltumiraErrorCodeCategory_instance();

    //! Return a short descriptive name for the category
    const char * name() const noexcept final;
    //! Return what each enum means in text
    std::string message( int p_condition ) const final;
    //! Allow generic ADAM error conditions to be compared to the specific errors above
    std::error_condition default_error_condition( int p_condition ) const noexcept final;
};
}    // end of anonymous namespace

AltumiraErrorCodeCategory & AltumiraErrorCodeCategory::AltumiraErrorCodeCategory_instance()
{
    static AltumiraErrorCodeCategory instance;
    return instance;
}

const char * AltumiraErrorCodeCategory::name() const noexcept
{
    return "AltumiraError";
}

std::string AltumiraErrorCodeCategory::message( int p_condition ) const
{
    switch( static_cast<AltumiraErrorCode>( p_condition ) )
    {
        case AltumiraErrorCode::CallFailed:
            return "The call failed";
        case AltumiraErrorCode::CreateHandleFailed:
            return "Failed to create handle";
        case AltumiraErrorCode::InvalidInstanceHandle:
            return "The supplied instance handle is invalid";
        case AltumiraErrorCode::OperationAbordedByUser:
            return "The operation was aborted by the user (by returning 0 from a progress callback function)";
        case AltumiraErrorCode::FatalError:
            return "A fatal error has occurred (e.g. out of memory). The library state is undefined and no further calls should be made.";
        case AltumiraErrorCode::CouldNotReadParameterFile:
            return "The parameter file could not be opened or read.";
        case AltumiraErrorCode::IllegalCommand:
            return "The prerequisites for calling this function has not been met.";
        case AltumiraErrorCode::InvalidInput:
            return "One or more parameters are invalid or out of range.";
        case AltumiraErrorCode::InvalidParameterFile:
            return "The supplied parameter file is invalid or corrupted.";
        case AltumiraErrorCode::LicenseError:
            return "There is no valid license for the requested operation.";
        case AltumiraErrorCode::UnsupportedParameters:
            return "The supplied parameters are not supported.";
        case AltumiraErrorCode::RequestedSettingNotSetUp:
            return "The requested setting has not been set up. Call CVIEEnhanceSetup() first.";
        case AltumiraErrorCode::UnknownError:
            return "An unknown error has occured.";
        case AltumiraErrorCode::GpuError:
            return "There was an error during GPU processing.";
        case AltumiraErrorCode::DefaultProcessingError:
            return "The call failed";
    }

    assert( "Missing value for enum AltumiraErrorCode in AltumiraErrorCodeCategory::message" );
    return "Unknown Altumira error";
}

std::error_condition AltumiraErrorCodeCategory::default_error_condition( int p_condition ) const noexcept
{
    switch( static_cast<AltumiraErrorCode>( p_condition ) )
    {
        case AltumiraErrorCode::CallFailed:
        case AltumiraErrorCode::CreateHandleFailed:
        case AltumiraErrorCode::InvalidInstanceHandle:
        case AltumiraErrorCode::OperationAbordedByUser:
        case AltumiraErrorCode::FatalError:
        case AltumiraErrorCode::CouldNotReadParameterFile:
        case AltumiraErrorCode::IllegalCommand:
        case AltumiraErrorCode::InvalidInput:
        case AltumiraErrorCode::InvalidParameterFile:
        case AltumiraErrorCode::LicenseError:
        case AltumiraErrorCode::UnsupportedParameters:
        case AltumiraErrorCode::RequestedSettingNotSetUp:
        case AltumiraErrorCode::UnknownError:
        case AltumiraErrorCode::GpuError:
        case AltumiraErrorCode::DefaultProcessingError:
            return make_error_condition( TomoErrorCondition::AltumiraImageProcessingError );
    }

    assert( "Missing value for enum AltumiraErrorCode in AltumiraErrorCodeCategory::default_error_condition" );
    return std::error_condition( p_condition, *this );    // I have no mapping for this code
}

//! Overload the global make_error_code() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_code make_error_code( AltumiraErrorCode p_error )
{
    return { static_cast<int>( p_error ), AltumiraErrorCodeCategory::AltumiraErrorCodeCategory_instance() };
}
