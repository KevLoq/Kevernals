#pragma once

#include "commons/Maths.h"

#include <QString>

struct AltumiraParameters
{
    AltumiraParameters(){};
    AltumiraParameters( const AltumiraParameters & p_other )
    {
        parameterFile = p_other.parameterFile;
        parameterFileSetting = p_other.parameterFileSetting;

        globalBrightness = p_other.globalBrightness;
        globalContrast = p_other.globalContrast;
        latitudeCompression = p_other.latitudeCompression;
        noiseReduction = p_other.noiseReduction;
        smallStructureEnhancement = p_other.smallStructureEnhancement;
    }

    ~AltumiraParameters() = default;

    bool operator==( const AltumiraParameters & p_rhs ) const
    {
        bool out = true;

        out &= p_rhs.parameterFile == parameterFile;
        out &= p_rhs.parameterFileSetting == parameterFileSetting;

        out &= AlmostEqualRelative( p_rhs.globalBrightness, globalBrightness );
        out &= AlmostEqualRelative( p_rhs.globalContrast, globalContrast );
        out &= AlmostEqualRelative( p_rhs.latitudeCompression, latitudeCompression );
        out &= AlmostEqualRelative( p_rhs.noiseReduction, noiseReduction );
        out &= AlmostEqualRelative( p_rhs.smallStructureEnhancement, smallStructureEnhancement );

        return out;
    }

    bool operator!=( const AltumiraParameters & rhs ) const
    {
        return !( operator==( rhs ) );
    }

    QString parameterFile;
    int parameterFileSetting{ 0 };

    float globalBrightness{ 0.0f };
    float globalContrast{ 0.0f };
    float latitudeCompression{ 0.0f };
    float noiseReduction{ 0.0f };
    float smallStructureEnhancement{ 0.0f };
};
