#pragma once

#include "commons/GeometryUtils.h"
#include "commons/ImageDataPtr.h"
#include "modules/geometry/TomoVolume.h"

#include <QPair>
#include <QString>

#include <memory>

struct PaveWithDensity
{
    Pave pave;
    float density;
    PaveWithDensity( const Pave & p_pave, float p_density )
      : pave( p_pave )
      , density( p_density )
    {}
};
struct SphereWithDensity
{
    Sphere sphere;
    float density;
    SphereWithDensity( const Sphere & p_sphere, float p_density )
      : sphere( p_sphere )
      , density( p_density )
    {}
};
struct CylinderWithDensity
{
    Cylinder cylinder;
    float density;
    CylinderWithDensity( const Cylinder & p_cylinder, float p_density )
      : cylinder( p_cylinder )
      , density( p_density )
    {}
};
class PhantomMaker
{
public:
    PhantomMaker() = delete;
    PhantomMaker( const TomoVolume * p_volume, float p_backgroundDensity = 50.f )
      : m_volume( p_volume )
      , m_backgroundDensity( p_backgroundDensity )
    {}

    ~PhantomMaker() = default;

    void AddPave( const Pave & p_pave, float p_density ) { m_paves.push_back( PaveWithDensity( p_pave, p_density ) ); }
    void AddSphere( const Sphere & p_sphere, float p_density ) { m_spheres.push_back( SphereWithDensity( p_sphere, p_density ) ); }
    void AddCylinder( const Cylinder & p_cylinder, float p_density ) { m_cylinders.push_back( CylinderWithDensity( p_cylinder, p_density ) ); }

    ImageDataPtr GetPhantom() const;

private:
    void ClearGeometryElements()
    {
        m_paves.clear();
        m_spheres.clear();
        m_cylinders.clear();
    }

    QVector<PaveWithDensity> m_paves;
    QVector<SphereWithDensity> m_spheres;
    QVector<CylinderWithDensity> m_cylinders;

    const TomoVolume * m_volume;
    float m_backgroundDensity{ 50. };
};
