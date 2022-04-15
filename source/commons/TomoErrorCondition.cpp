#include "commons/TomoErrorCondition.h"

#include <cassert>

namespace    // anonymous namespace
{
//! Define a custom error code category derived from std::error_category
class TomoErrorConditionCategory : public std::error_category
{
public:
    //! Declare a global function returning a static instance of the custom category
    //! Note: the uniqueness of categories is guaranteed by comparing addresses, so it is achieved with a static object
    //! Note: in C++11 initialisation of static local variables is guaranteed to be thread-safe
    static TomoErrorConditionCategory & TomoErrorConditionCategory_instance();
    //! Return a short descriptive name for the category
    const char * name() const noexcept final;
    //! Return what each enum means in text
    std::string message( int p_condition ) const final;
};
}    // end of anonymous namespace

TomoErrorConditionCategory & TomoErrorConditionCategory::TomoErrorConditionCategory_instance()
{
    static TomoErrorConditionCategory instance;
    return instance;
}

const char * TomoErrorConditionCategory::name() const noexcept
{
    return "Error";
}

std::string TomoErrorConditionCategory::message( int p_condition ) const
{
    switch( static_cast<TomoErrorCondition>( p_condition ) )
    {
        case TomoErrorCondition::CouldNotImportDicomFile:
            return "Could not import DICOM file";
        case TomoErrorCondition::DatabaseError:
            return "Database error";
        case TomoErrorCondition::RadonMatrixError:
            return "Radon Matrix error";
        case TomoErrorCondition::TomosynthesisReconstructorError:
            return "Tomosynthesis reconstruction error";
        case TomoErrorCondition::TomoGeometryError:
            return "Tomosynthesis Geometry error";
        case TomoErrorCondition::AltumiraImageProcessingError:
            return "Altumira image processing error";
    }
    assert( "Missing value for TomoErrorConditionCategory enum in TomoErrorConditionCategory::message" );
    return "Unknown error";
}

std::error_condition make_error_condition( TomoErrorCondition p_error )
{
    return { static_cast<int>( p_error ), TomoErrorConditionCategory::TomoErrorConditionCategory_instance() };
}
