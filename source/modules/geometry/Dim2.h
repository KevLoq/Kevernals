#pragma once 

#include "commons/Maths.h"
#include "commons/GlobalUtils.h"

#include <string>
#include <functional>
#include <array>
#include <vector> 
#include <type_traits>
#include <numeric>
#include <limits>
#include <optional>

template<typename T>
struct Dim2
{ 
    Dim2() = delete;
 
    Dim2( T const & p_x, T const & p_y)
    { 
        x = p_x;
        y = p_y;
    } 
 
    Dim2( Dim2<T> const & p_original)
    { 
        x = p_original.x;
        y = p_original.y;
    } 

    virtual ~Dim2() = default;

    Dim2 & operator*=( T p_lambda )
    {
        x *= p_lambda;
        y *= p_lambda;
        return *this;
    }

    Dim2 & operator=( Dim2<T> const & p_original )
    {
        x = p_original.x;
        y = p_original.y;
        return *this;
    }
 
    friend Dim2 operator*( T p_lambda, Dim2 p_rhs )
    {
        p_rhs *= p_lambda;
        return p_rhs;
    }

    friend Dim2 operator*( Dim2 p_lhs, T p_lambda )
    {
        p_lhs *= p_lambda;
        return p_lhs;
    }
 
    friend bool operator==( Dim2 const & p_lhs, Dim2 const & p_rhs )
    {
        return AlmostEqualRelative( static_cast<double>(p_lhs.x), static_cast<double>(p_rhs.x) ) 
            && AlmostEqualRelative( static_cast<double>(p_lhs.y), static_cast<double>(p_rhs.y) );
    }

    friend bool operator!=( Dim2 const & p_lhs, Dim2 const & p_rhs )
    {
        return !( p_lhs == p_rhs );
    }

    static std::optional<Dim2> FromStringArray( std::array<std::string,glob::TWO> const & p_coordinates )
    {
        Dim2 position(0,0); 
        try {
            position.x =  static_cast<T>(std::stod(p_coordinates[0]));
            position.y =  static_cast<T>(std::stod(p_coordinates[1]));
        }
        catch (const std::invalid_argument & e) {
            return std::nullopt;
        }
        catch (const std::out_of_range & e) {
            return std::nullopt;
        }
        return position;
    }
 
    static std::optional<Dim2> FromString( std::string const & p_coordinates )
    {
        auto arrayOfCoordinates = glob::SentenceToArray2_OfWords(p_coordinates) ;
        if(!arrayOfCoordinates.has_value()){
            return std::nullopt;
        }
        return Dim2<T>::FromStringArray(arrayOfCoordinates.value());
    }

    T x{ 0 };
    T y{ 0 };

    int const size = glob::TWO;
};

template<typename T>
std::ostream &operator<<(std::ostream &p_outputStream, Dim2<T> const & p_data) { 
    return p_outputStream << "(" << p_data.x << "," << p_data.y << ")";
}