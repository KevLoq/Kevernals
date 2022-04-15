#pragma once

//
// WARNING
//
// As std::error_category instances must be globally unique in a process as per the C++ standard,
// the definition of a custom error_category in a shared library is the only standards conforming one.
// Any other setting (header only or static library) may lead to more than one instance of the singleton,
// this can cause misoperation especially during error condition comparisons
//

// Note: we do not actually need the error code logger here, but every file including this header will do
#include "commons/PrintErrorCode.h"

#include <system_error>

enum class TomoErrorCondition
{
    CouldNotImportDicomFile = 1,
    RadonMatrixError,
    DatabaseError,
    DICOMReadError,
    TomoGeometryError,
    TomosynthesisReconstructorError,
    AltumiraImageProcessingError,
};
// REM KL: if possible, the TomoErrorConditionError naming should be uniformized

namespace std
{
//! Tell the C++ 11 STL metaprogramming that enum TomoErrorCondition is registered with the standard error condition system
template<>
struct is_error_condition_enum<TomoErrorCondition> : public std::true_type
{
};
}    // end of namespace std

//! Overload the global make_error_condition() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_condition make_error_condition( TomoErrorCondition p_error );
