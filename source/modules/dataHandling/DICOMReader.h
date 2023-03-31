#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"

#include <string>
#include <vector>

class TomoGeometry;
class DICOMReader
{
public:
    DICOMReader() = delete;
    DICOMReader( TomoGeometry * p_tomoGeometry );

    Result<ImageDataPtr> Read( std::string const & p_dicomFilePath ) const;
    Result<ImageDataPtr> ReadDirectory( std::string const & p_dicomFilesContainedDirPath, std::vector<int> const & p_indicesToRemove ) const;

private:
    struct ImageDataPtrAndTimeStamp
    {
        ImageDataPtr image;
        std::string acquisitionTime;
        std::string fileName;
    };

    TomoGeometry * m_tomoGeometry;
};
