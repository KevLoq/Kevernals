#pragma once 

#include "commons/GlobalUtils.h"
#include "commons/Maths.h"

#include <string> 
#include <array>  
#include <numeric>
#include <optional>

template<typename T>
struct Dim3
{    
    Dim3() = delete;
 
    Dim3( T const & p_x, T const & p_y, T const & p_z)
    { 
        x = p_x;
        y = p_y;
        z = p_z;
    } 

    Dim3( Dim3<T> const & p_original)
    { 
        x = p_original.x;
        y = p_original.y;
        z = p_original.z;
    } 

    virtual ~Dim3() = default;
 
    Dim3 & operator*=( T p_lambda )
    {
        x *= p_lambda;
        y *= p_lambda;
        z *= p_lambda;
        return *this;
    }
    Dim3 & operator=( Dim3<T> const & p_original)
    { 
        x = p_original.x;
        y = p_original.y;
        z = p_original.z;
        return *this;
    } 

    friend Dim3 operator*( T p_lambda, Dim3 p_rhs )
    {
        p_rhs *= p_lambda;
        return p_rhs;
    }

    friend Dim3 operator*( Dim3 p_lhs, T p_lambda )
    {
        p_lhs *= p_lambda;
        return p_lhs;
    }

    friend bool operator==( Dim3 const & p_lhs, Dim3 const & p_rhs )
    {
        return      AlmostEqualRelative( static_cast<double>(p_lhs.x), static_cast<double>(p_rhs.x) )
                 && AlmostEqualRelative( static_cast<double>(p_lhs.y), static_cast<double>(p_rhs.y) )
                 && AlmostEqualRelative( static_cast<double>(p_lhs.z), static_cast<double>(p_rhs.z) );
    }

    friend bool operator!=( Dim3 const & p_lhs, Dim3 const & p_rhs )
    {
        return !( p_lhs == p_rhs );
    }

    static std::optional<Dim3> FromStringArray( std::array<std::string,glob::THREE> const & p_coordinates )
    {
        Dim3 position(0,0,0); 
        try {
            position.x =  static_cast<T>(std::stod(p_coordinates[0]));
            position.y =  static_cast<T>(std::stod(p_coordinates[1]));
            position.z =  static_cast<T>(std::stod(p_coordinates[2]));
        }
        catch (const std::invalid_argument & e) {
            return std::nullopt;
        }
        catch (const std::out_of_range & e) {
            return std::nullopt;
        }  
        return position;
    }
 
    static std::optional<Dim3> FromString( std::string const & p_coordinates )
    {
        auto arrayOfCoordinates = glob::SentenceToArray3_OfWords(p_coordinates) ;
        if(!arrayOfCoordinates.has_value()) {
            return std::nullopt;
        }
        return Dim3<T>::FromStringArray(arrayOfCoordinates.value());
    }

    T x{ 0 };
    T y{ 0 };
    T z{ 0 };
    
    int const size = glob::THREE;
};

template<typename T>
std::ostream &operator<<(std::ostream &p_outputStream, Dim3<T> const & p_data) { 
    return p_outputStream << "(" << p_data.x << "," << p_data.y << "," << p_data.z << ")";
}