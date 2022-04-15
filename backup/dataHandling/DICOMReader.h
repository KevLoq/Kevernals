#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"

#include <QString>

class TomoGeometry;
class DICOMReader
{
public:
    DICOMReader() = delete;
    DICOMReader( TomoGeometry * p_tomoGeometry );

    Result<ImageDataPtr> Read( const QString & p_dicomFilePath ) const;

private:
    TomoGeometry * m_tomoGeometry;
};
