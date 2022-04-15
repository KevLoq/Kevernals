#pragma once

#include "commons/ImageDataPtr.h"
#include "commons/Result.h"
#include "modules/altumira/AltumiraErrorCode.h"
#include "modules/altumira/AltumiraParameters.h"

#include <QString>
#include <cvie.h>
#include <xray-3_tp.h>

#include <memory>
#include <string>

//! Interface for image processing algorithms. Altumira's initialisation code is done in constructor
//! and clean up in destructor.
//! This ImageProcessing object is meant to be kept as a member of a class needing procesing.
//! Methods LoadParameterFile, InitialiseSettings and PerformEnhancement are meant to be called in this order.
class AltumiraImageProcessing
{
public:
    AltumiraImageProcessing();
    ~AltumiraImageProcessing() noexcept;

    //! Create a new instance object used for processing
    void CreateNewInstance();

    //! Load parameter file located at p_pathToParameterFile and returns the number of settings
    //! contained in this file into p_numberOfSettings
    Result<void> LoadParameterFile( const QString & p_parameterFilename, int & p_numberOfSettings ) const;

    //! Initialise given p_setting for image buffer contained in p_instanceDTO
    Result<void> InitialiseSettings( int p_setting, const int p_imageWidth, const int p_imageHeight );

    Result<void> ParametersSettings( AltumiraParameters p_parameters );

    //! Perform processing to enhance image based on parameter file and chosen setting
    Result<void> PerformEnhancement( const ImageDataPtr & p_inputImageData, const ImageDataPtr & p_outputImageData ) const;

private:
    //! Return error code if the Altumira function given in first parameter fails
    //! Note: Altumira is a C API, it does not throw exceptions. We need to make it explicit to that the compiler
    //! knows that our destructor cannot throw
    Result<void> ExecuteAltumiraFunction( ECVIE p_cvieResult ) const noexcept;

    //! Return error code if the Altumira function has errors during activation key process.
    Result<void> SetAltumiraActivationKey( const int p_altumiraLicenseMethodIndex, const QString & activationKey );

    //! Instance handle for Altumira algorithm
    HCVIE m_instanceHandle{ nullptr };
    //! Index of the setting currently loaded from parameter file
    int m_currentLoadedSetting{ -1 };

    //! Is GPU processing enabled in preferences.ini
    bool m_isGpuProcessingEnabled{ true };
    //! Is verbose mode enabled in preferences.ini
    bool m_isVerboseEnabled{ false };
    //! Number of thread used for CPU processing requested in preferences.ini
    int m_altumiraProcessingThreadNumber{ 1 };
};
