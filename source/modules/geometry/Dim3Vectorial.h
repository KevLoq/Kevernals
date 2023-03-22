#pragma once  

#include "modules/geometry/Dim3.h"
 

template<typename T>
struct Dim3Vectorial : public Dim3<T>
{   
    Dim3Vectorial( T const & p_x, T const & p_y, T const & p_z) : Dim3<T>(p_x,p_y,p_z) {}

    virtual ~Dim3Vectorial() = default;
 
    Dim3Vectorial & operator+=( Dim3Vectorial const & p_rhs )
    {
        x += p_rhs.x;
        y += p_rhs.y; 
        z += p_rhs.z; 
        return *this;
    }

    Dim3Vectorial & operator-=( Dim3Vectorial const & p_rhs )
    {
        x -= p_rhs.x;
        y -= p_rhs.y; 
        z -= p_rhs.z; 
        return *this;
    }
  
    friend Dim3Vectorial operator+( Dim3Vectorial p_lhs, Dim3Vectorial const & p_rhs )
    {
        p_lhs += p_rhs;
        return p_lhs;
    }

    friend Dim3Vectorial operator-( Dim3Vectorial p_lhs, Dim3Vectorial const & p_rhs )
    {
        p_lhs -= p_rhs;
        return p_lhs;
    }
     
};

 