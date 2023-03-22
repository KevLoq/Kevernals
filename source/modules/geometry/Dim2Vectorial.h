#pragma once  

#include "modules/geometry/Dim2.h"
 

template<typename T>
struct Dim2Vectorial : public Dim2<T>
{   
    Dim2Vectorial( T const & p_x, T const & p_y) : Dim2<T>(p_x,p_y) {}

    virtual ~Dim2Vectorial() = default;
 
    Dim2Vectorial & operator+=( Dim2Vectorial const & p_rhs )
    {
        x += p_rhs.x;
        y += p_rhs.y;
        return *this;
    }

    Dim2Vectorial & operator-=( Dim2Vectorial const & p_rhs )
    {
        x -= p_rhs.x;
        y -= p_rhs.y; 
        return *this;
    }
  
    friend Dim2Vectorial operator+( Dim2Vectorial p_lhs, Dim2Vectorial const & p_rhs )
    {
        p_lhs += p_rhs;
        return p_lhs;
    }

    friend Dim2Vectorial operator-( Dim2Vectorial p_lhs, Dim2Vectorial const & p_rhs )
    {
        p_lhs -= p_rhs;
        return p_lhs;
    }
     
};

 