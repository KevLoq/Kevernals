#pragma once

#include "commons/Result.h"
#include "modules/geometry/TomoGeometry.h"

#include <QString>


//! Interface for image processing algorithms. Altumira's initialisation code is done in constructor
//! and clean up in destructor.
//! This ImageProcessing object is meant to be kept as a member of a class needing procesing.
//! Methods LoadParameterFile, InitialiseSettings and PerformEnhancement are meant to be called in this order.
class TomoGeometryParser
{
public:
    TomoGeometryParser() = delete;
    ~TomoGeometryParser() = delete;

    //! Create a new instance object used for processing
    static Result<TomoGeometry> ParseXmlFile( const QString & p_xmlGeometryFilePath );
};
