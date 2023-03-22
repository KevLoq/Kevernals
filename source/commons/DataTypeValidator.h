#pragma once

#include <numeric>

template<typename T>
inline constexpr bool IsUnsignedShort()
{
    return std::numeric_limits<T>::is_integer
           && !std::numeric_limits<T>::is_signed
           && std::numeric_limits<T>::max() == USHRT_MAX;
}

template<typename T>
inline constexpr bool IsUnsignedChar()
{
    return std::numeric_limits<T>::is_integer
           && !std::numeric_limits<T>::is_signed
           && std::numeric_limits<T>::max() == UCHAR_MAX;
}

template<typename T>
inline constexpr bool IsInt()
{ 
    return std::numeric_limits<T>::is_integer
           && std::numeric_limits<T>::is_signed
           && std::numeric_limits<T>::max() == INT_MAX;
}

template<typename T>
inline constexpr bool IsLong()
{
    return std::numeric_limits<T>::is_integer
           && std::numeric_limits<T>::is_signed
           && std::numeric_limits<T>::max() == LONG_MAX;
}

template<typename T>
inline constexpr bool IsFloat()
{
    return std::numeric_limits<T>::has_infinity
           && std::numeric_limits<T>::infinity() == HUGE_VALF;
}

template<typename T>
inline constexpr bool IsDouble()
{
    return std::numeric_limits<T>::has_infinity
           && std::numeric_limits<T>::infinity() == HUGE_VAL;
}


template<typename T>
inline constexpr bool IsLongDouble()
{
    return std::numeric_limits<T>::has_infinity
           && std::numeric_limits<T>::infinity() == HUGE_VALL;
}