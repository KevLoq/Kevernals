#pragma once


#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numeric>
#include <type_traits>
#include <vector>
template<int ROW, int COL>
using FloatMatrix = std::array<std::array<float, COL>, ROW>;

inline double pi()
{
    return std::atan( 1 ) * 4.;
}

inline float piFloat()
{
    return std::atan( 1.f ) * 4.f;
}

// Compare 2 float with an error range
inline bool AlmostEqualRelative( float A, float B, float maxRelDiff = std::numeric_limits<float>::epsilon() )
{
    // Calculate the difference.
    const float diff = std::fabs( A - B );
    A = std::fabs( A );
    B = std::fabs( B );
    const float scaledEpsilon = maxRelDiff * std::max( A, B );

    return ( diff <= scaledEpsilon );
}

// Compare 2 double with an error range
inline bool AlmostEqualRelative( double A, double B, double maxRelDiff = std::numeric_limits<double>::epsilon() )
{
    // Calculate the difference.
    const double diff = std::fabs( A - B );
    A = std::fabs( A );
    B = std::fabs( B );
    const double scaledEpsilon = maxRelDiff * std::max( A, B );

    return ( diff <= scaledEpsilon );
}

// Compare 2 long double with an error range
inline bool AlmostEqualRelative( long double A, long double B, long double maxRelDiff = std::numeric_limits<long double>::epsilon() )
{
    // Calculate the difference.
    const long double diff = std::fabs( A - B );
    A = std::fabs( A );
    B = std::fabs( B );
    const long double scaledEpsilon = maxRelDiff * std::max( A, B );

    return ( diff <= scaledEpsilon );
}
