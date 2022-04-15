#pragma once

//
// WARNING
//
// As std::error_category instances must be globally unique in a process as per the C++ standard,
// the definition of a custom error_category in a shared library is the only standards conforming one.
// Any other setting (header only or static library) may lead to more than one instance of the singleton,
// this can cause misoperation especially during error condition comparisons
//
#include "commons/TomoErrorCondition.h"

#include <system_error>

//! The enum class cannot be into adam namespace. Otherwise make_error_code below must be in adam namespace too
//! to be found by Argument-Dependent Lookup by the compiler. However make_error_code is then not found by linker
//! (unresolved external symbol). It is possible that I missed something (OC) but we will define our custom enum
//! for errors outside adam namespace
enum class DICOMReaderErrorCode
{
    DataFileReadingProblem,
};

namespace std
{
//! Tell the C++ 11 STL metaprogramming that the enum above is registered with the standard error code system
template<>
struct is_error_code_enum<DICOMReaderErrorCode> : public std::true_type
{
};
}    // end of namespace std

//! Overload the global make_error_code() free function with our custom enum.
//! It will be found via ADL (Argument-Dependent Lookup) by the compiler if needed.
std::error_code make_error_code( DICOMReaderErrorCode p_error );
