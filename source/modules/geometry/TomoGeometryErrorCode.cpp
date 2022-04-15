#include "modules/geometry/TomoGeometryErrorCode.h"

#include "commons/TomoErrorCondition.h"

#include <cassert>

namespace    // anonymous namespace
{
//! Define a custom error code category derived from std::error_category
class TomoGeometryErrorCodeCategory : public std::error_category
{
public:
    //! Declare a global function returning a static instance of the custom category
    //! Note: the uniqueness of categories is guaranteed by comparing addresses, so it is achieved with a static object
    //! Note: in C++11 initialisation of static local variables is guaranteed to be thread-safe
    static TomoGeometryErrorCodeCategory & TomoGeometryErrorCodeCategory_instance();

    //! Return a short descriptive name for the category
    const char * name() const noexcept final;
    //! Return what each enum means in text
    std::string message( int p_condition ) const final;
    //! Allow generic ADAM error conditions to be compared to the specific errors above
    std::error_condition default_error_condition( int p_condition ) const noexcept final;
};
}    // end of anonymous namespace

TomoGeometryErrorCodeCategory & TomoGeometryErrorCodeCategory::TomoGeometryErrorCodeCategory_instance()
{
    static TomoGeometryErrorCodeCategory instance;
    return instance;
}

const char * TomoGeometryErrorCodeCategory::name() const noexcept
{
    return "TomoGeometryError";
}

std::string TomoGeometryErrorCodeCategory::message( int p_condition ) const
{
    switch( static_cast<TomoGeometryErrorCode>( p_condition ) )
    {
        case TomoGeometryErrorCode::XraySourcesPositionParsingError:
            return "Parsing the sources position in geometry xml file failed";
        case TomoGeometryErrorCode::DetectorPositionsParsingError:
            return "Parsing the detector positions in geometry xml file failed";
        case TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError:
            return "Parsing the reconstruction volume geometry in geometry xml file failed";
        case TomoGeometryErrorCode::ProjectionRoisParsingError:
            return "Parsing the projections Rois in geometry xml file failed";
        case TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError:
            return "Conversion from a QString list to position in world failed";
        case TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError:
            return "Conversion from a QString list to quadrilateral pixels coordinates failed";
        case TomoGeometryErrorCode::QStringToSize3DConversionError:
            return "Conversion from a QString list to volume size failed";
        case TomoGeometryErrorCode::FileToParseError:
            return "Error with the geometry file to parse";
        case TomoGeometryErrorCode::FulcrumParsingError:
            return "Parsing the fulcrum position in geometry xml file failed";
    }

    assert( "Missing value for enum TomoGeometryErrorCode in TomoGeometryErrorCodeCategory::message" );
    return "Unknown TomoGeometry error";
}

std::error_condition TomoGeometryErrorCodeCategory::default_error_condition( int p_condition ) const noexcept
{
    switch( static_cast<TomoGeometryErrorCode>( p_condition ) )
    {
        case TomoGeometryErrorCode::XraySourcesPositionParsingError:
        case TomoGeometryErrorCode::DetectorPositionsParsingError:
        case TomoGeometryErrorCode::ReconstructionVolumeGeometryParsingError:
        case TomoGeometryErrorCode::ProjectionRoisParsingError:
        case TomoGeometryErrorCode::FileToParseError:
        case TomoGeometryErrorCode::FulcrumParsingError:
        case TomoGeometryErrorCode::QStringToPositionIn3DWorldConversionError:
        case TomoGeometryErrorCode::QStringToSize3DConversionError:
        case TomoGeometryErrorCode::QStringToQuadrilateral2DConversionError:
            return make_error_condition( TomoErrorCondition::TomoGeometryError );
    }

    assert( "Missing value for enum TomoGeometryErrorCode in TomoGeometryErrorCodeCategory::default_error_condition" );
    return std::error_condition( p_condition, *this );    // I have no mapping for this code
}

//! Overload the global make_error_code() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_code make_error_code( TomoGeometryErrorCode p_error )
{
    return { static_cast<int>( p_error ), TomoGeometryErrorCodeCategory::TomoGeometryErrorCodeCategory_instance() };
}
