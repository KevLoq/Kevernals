#include "modules/altumira/AltumiraImageProcessing.h"

#include "commons/PrintErrorCode.h"

#include <QDebug>

#include <algorithm>
#include <sstream>

Result<void> AltumiraImageProcessing::ExecuteAltumiraFunction( ECVIE p_cvieResult ) const noexcept
{
    if( p_cvieResult != CVIE_E_OK )
    {
        //char errStr[256];
        //qDebug() << "C++: AltumiraImageProcessing::ExecuteAltumiraFunction" << CVEMGetLastError( m_instanceHandle, errStr, sizeof( errStr ) );
        // Create an std::error_code from ECVIE's error code
        auto errorCode = make_error_code( static_cast<AltumiraErrorCode>( p_cvieResult ) );
        return errorCode;
    }
    return outcome::success();
}

AltumiraImageProcessing::AltumiraImageProcessing()
{
    HCVIE handle = nullptr;
    bool activationUsingDefaultLicenseMethodSucceeded = false;

    const QString myActivationKey = "DAr7RhlyvT8fT#V5";
    const QString myCPID = "000000CND05056GZ";

    // Get possible license methods
    const char ** licenseMethodNames;
    if( auto result1 = this->ExecuteAltumiraFunction( CVLMGetPossibleHostTypes( handle, &licenseMethodNames ) ) )    // handle is not used by CVLMGetPossibleHostTypes
    {
        // To cater for the possibility of having multiple license methods now or in the future, the licenseMethodIndex parameter of CVLMGetHostId
        // should not be a constant in the program but be retrieved using the CVLMGetPossibleHostTypes function.
        // This function sets up a pointer to an array of strings which must be traversed to see which index a given string name has.
        const char * defaultLicenseMethod = "Rockey4ND";
        //        const char * devLicenseMethod = "CPID";
        int indexDefaultLicenseMethod = -1;
        //        int indexDevLicenseMethod = -1;
        for( int index = 0; licenseMethodNames[index][0] != '\0'; index++ )
        {
            if( strcmp( licenseMethodNames[index], defaultLicenseMethod ) == 0 )
            {
                indexDefaultLicenseMethod = index;
                qInfo() << "Rockey4ND license method is available for Altumira";
            }
            //            if( strcmp( licenseMethodNames[index], devLicenseMethod ) == 0 )
            //            {
            //                indexDevLicenseMethod = index;
            //                qInfo()  << "CPID license method is available for Altumira";
            //            }
        }

        // If the default license method exists
        if( indexDefaultLicenseMethod != -1 )
        {
            if( auto setAltumiraActvationKeyResult = SetAltumiraActivationKey( indexDefaultLicenseMethod, myActivationKey ) )
            {
                activationUsingDefaultLicenseMethodSucceeded = true;
            }
            else
            {
                // Error already logged inside function.
            }
        }
        else
        {
            qWarning() << "Altumira: Could not find Rockey4ND as available license method";
        }
    }
    else
    {
        qCritical() << QString::fromStdString( PrintErrorCode( result1.error(), "Altumira: Could not retrieve the list of license methods" ) );
    }


    // If default activation using dongle key has failed, try to activate the license using a CPID
    if( !activationUsingDefaultLicenseMethodSucceeded )
    {
        QByteArray imageProcessingCpidArray = myCPID.toLocal8Bit();
        char * imageProcessingCpidPtr = imageProcessingCpidArray.data();
        if( auto result = this->ExecuteAltumiraFunction( CVLMSetParameterValue( LM_INIT, imageProcessingCpidPtr ) ) )
        {
            qInfo() << "A cpid was successfully set up for Altumira";

            const char * devLicenseMethod = "CPID";
            int indexDevLicenseMethod = -1;
            for( int index = 0; licenseMethodNames[index][0] != '\0'; index++ )
            {
                if( strcmp( licenseMethodNames[index], devLicenseMethod ) == 0 )
                {
                    indexDevLicenseMethod = index;
                    qInfo() << "Altumira: CPID license method is available for Altumira";
                }
            }

            if( indexDevLicenseMethod != -1 )
            {
                auto setAltumiraActvationKeyResult = SetAltumiraActivationKey( indexDevLicenseMethod, myActivationKey );
                if( !setAltumiraActvationKeyResult )
                {
                    // Error already logged inside function.
                }
            }
            else
            {
                qWarning() << "Altumira: Could not find CPID as available license method";
            }
        }
        else
        {
            qCritical() << QString::fromStdString( PrintErrorCode( result.error(), "Altumira: Could not set a cpid" ) );
        }
    }

    // Select options for processing based on settings
    m_isGpuProcessingEnabled = true;
    m_isVerboseEnabled = true;
    m_altumiraProcessingThreadNumber = 0;

    // Create an instance for Altumira
    this->CreateNewInstance();
}

AltumiraImageProcessing::~AltumiraImageProcessing() noexcept
{
    try
    {
        if( auto result = this->ExecuteAltumiraFunction( CVIEDestroy( &m_instanceHandle ) ) )
        {
            qInfo() << "Altumira: The instance for Altumira was successfully destroyed";
        }
        else
        {
            qCritical() << QString::fromStdString( PrintErrorCode( result.error(), "Altumira: Could not destroy Altumira instance" ) );
        }
    }
    catch( ... )
    {
        // For the compiler, LOG may throw an exception (triggers clang-tidy exception-escape warning)
        //        qCritical() << "~AltumiraImageProcessing() threw an exception";
    }
}

void AltumiraImageProcessing::CreateNewInstance()
{
    if( m_instanceHandle != nullptr )
    {
        auto result = this->ExecuteAltumiraFunction( CVIEDestroy( &m_instanceHandle ) );
        if( result.has_error() )
        {
            qCritical() << QString::fromStdString( PrintErrorCode( result.error(), "Could not destroy Altumira instance" ) );
        }
    }

    int createFlags = m_isGpuProcessingEnabled ? CVIE_CREATE_CUDA : CVIE_CREATE_DEFAULT;
    std::string processingType = m_isGpuProcessingEnabled ? "GPU" : "CPU";

    Result<void> result{ outcome::failure( AltumiraErrorCode::UnknownError ) };
    if( m_isVerboseEnabled )
    {
        result = this->ExecuteAltumiraFunction( CVIECreate( &m_instanceHandle, createFlags | CVIE_CREATE_TRACE ) );
        if( result )
        {
            qInfo() << "A " << QString::fromStdString( processingType ) << " instance for Altumira was successfully created";
        }
        else
        {
            std::string additionalComment = "Could not create a " + processingType + " instance with trace enabled for Altumira ";
            qCritical() << QString::fromStdString( PrintErrorCode( result.error(), additionalComment ) );
        }
    }
    else
    {
        result = this->ExecuteAltumiraFunction( CVIECreate( &m_instanceHandle, createFlags ) );
        if( result )
        {
            qInfo() << "A " << QString::fromStdString( processingType ) << " instance for Altumira was successfully created with trace enabled";
        }
        else
        {
            std::string additionalComment = "Could not create a " + processingType + " instance for Altumira ";
            qCritical() << QString::fromStdString( PrintErrorCode( result.error(), additionalComment ) );
        }
    }
    result = this->ExecuteAltumiraFunction( CVIESetThreads( m_instanceHandle, m_altumiraProcessingThreadNumber ) );
    if( result.has_error() )
    {
        std::string additionalComment = "Failed to set processing to use ";
        qCritical() << QString::fromStdString( PrintErrorCode( result.error(), additionalComment ) ) << QString::number( m_altumiraProcessingThreadNumber ) << " Threads";
    }
}
//
// REM KL : p_numberOfSettings is neved used in the using context of LoadParameterFile method (in PipelineController)
Result<void> AltumiraImageProcessing::LoadParameterFile( const QString & p_parameterFilename, int & p_numberOfSettings ) const
{
    // Full path to parameter file
    QString filePath = "resources/" + p_parameterFilename;
    QByteArray filePathUtf8 = filePath.toUtf8();    // convert to QByteArrray, do not use directly toUtf8().data() because it doesn't work.
    char * parameterFile = filePathUtf8.data();
    //qDebug() << "C++: AltumiraImageProcessing::LoadParameterFile" << QString::fromLocal8Bit( parameterFile ) << filePath << p_numberOfSettings << parameterFile;

    // Load parameter file that defines the processing algorithm and its settings
    auto result = this->ExecuteAltumiraFunction( CVIESetParameterFile( m_instanceHandle, parameterFile, &p_numberOfSettings ) );
    if( result )
    {
        qInfo() << "Parameter file " << p_parameterFilename << " containing " << QString::number( p_numberOfSettings ) << " settings for Altumira was successfully loaded";
    }
    else
    {
        std::string additionalComment = "Could not load parameter file " + p_parameterFilename.toStdString() + " for Altumira";
        qCritical() << QString::fromStdString( PrintErrorCode( result.error(), additionalComment ) );
    }

    return result;
}

Result<void> AltumiraImageProcessing::InitialiseSettings( int p_setting, const int p_imageWidth, const int p_imageHeight )
{
    //qDebug() << "C++: AltumiraImageProcessing::InitialiseSettings" << p_setting << p_imageWidth << p_imageHeight;
    // Initialise the processing settings
    // Note: the image is always allocated on 16 bit per pixel (even when the image is actually using less bit per pixel)
    auto result = this->ExecuteAltumiraFunction( CVIEEnhanceSetup( m_instanceHandle, p_imageWidth, p_imageHeight, CVIE_DATA_U16, p_setting, nullptr ) );
    if( result )
    {
        qInfo() << "The Altumira setting " << p_setting << " was successfully initialised for an image " << p_imageWidth << "x" << p_imageHeight;
        m_currentLoadedSetting = p_setting;
    }
    else
    {
        std::string additionalComment = "Could not initialise Altumira setting " + std::to_string( p_setting ) + " for an image " + std::to_string( p_imageWidth ) + "x" + std::to_string( p_imageHeight );
        qCritical() << QString::fromStdString( PrintErrorCode( result.error(), additionalComment ) );
    }

    return result;
}

Result<void> AltumiraImageProcessing::ParametersSettings( AltumiraParameters p_parameters )
{
    //qDebug() << "C++: AltumiraImageProcessing::ParametersSettings" << p_parameters.parameterFileSetting << p_parameters.globalBrightness << m_instanceHandle;

    auto result = this->ExecuteAltumiraFunction( CVIESetParameterValue( m_instanceHandle, p_parameters.parameterFileSetting, XRAY3_TP_GLOBAL_BRIGTHNESS, &p_parameters.globalBrightness ) );
    if( !result )
    {
        auto errorCode = result.error();
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return result;
    }
    result = this->ExecuteAltumiraFunction( CVIESetParameterValue( m_instanceHandle, p_parameters.parameterFileSetting, XRAY3_TP_GLOBAL_CONTRAST, &p_parameters.globalContrast ) );
    if( !result )
    {
        auto errorCode = result.error();
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return result;
    }
    result = this->ExecuteAltumiraFunction( CVIESetParameterValue( m_instanceHandle, p_parameters.parameterFileSetting, XRAY3_TP_LATITUDE_COMPRESSION, &p_parameters.latitudeCompression ) );
    if( !result )
    {
        auto errorCode = result.error();
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return result;
    }
    result = this->ExecuteAltumiraFunction( CVIESetParameterValue( m_instanceHandle, p_parameters.parameterFileSetting, XRAY3_TP_NOISE_REDUCTION, &p_parameters.noiseReduction ) );
    if( !result )
    {
        auto errorCode = result.error();
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return result;
    }
    result = this->ExecuteAltumiraFunction( CVIESetParameterValue( m_instanceHandle, p_parameters.parameterFileSetting, XRAY3_TP_SMALL_STRUCTURE_ENHANCEMENT, &p_parameters.smallStructureEnhancement ) );
    if( !result )
    {
        auto errorCode = result.error();
        qCritical() << QString::fromStdString( PrintErrorCode( errorCode ) );
        return result;
    }

    return outcome::success();
}

Result<void> AltumiraImageProcessing::PerformEnhancement( const ImageDataPtr & p_inputImageData, const ImageDataPtr & p_outputImageData ) const
{
    if( p_inputImageData != nullptr )
    {
        auto internalImagePointer = static_cast<unsigned short *>( p_inputImageData->GetScalarPointer() );
        auto outputImagePointer = static_cast<unsigned short *>( p_outputImageData->GetScalarPointer() );

        //int * intputDimensions = p_inputInstanceDTO->GetRawPixels()->GetDimensions();
        //int * outputDimensions = p_outputImageData->GetDimensions();
        //auto inputDataType = p_inputInstanceDTO->GetRawPixels()->GetScalarTypeAsString();
        //auto outputDataType = p_outputImageData->GetScalarTypeAsString();

        //LOG( DEBUG ) << "AltumiraImageProcessing::PerformEnhancement: Input: " << intputDimensions[0] << "x" << intputDimensions[1] << inputDataType << " Output: " << outputDimensions[0] << "x" << outputDimensions[1] << outputDataType;

        // Check if static_casts did not fail
        if( internalImagePointer && outputImagePointer )
        {
            auto result = this->ExecuteAltumiraFunction( CVIEEnhanceNext( m_instanceHandle, internalImagePointer, outputImagePointer, m_currentLoadedSetting ) );

            if( result )
            {
                //                qInfo()  << "The image "
                //                            << p_inputInstanceDTO->GetSopInstanceUID().toStdString()
                //                            << " was successfully processed by Altumira";
            }
            else
            {
                std::string additionalComment = "Could not process the image with Altumira";
                qCritical() << QString::fromStdString( PrintErrorCode( result.error(), additionalComment ) );
            }

            return result;
        }

        auto errorCode = make_error_code( AltumiraErrorCode::InvalidInput );
        qWarning() << QString::fromStdString( PrintErrorCode( errorCode ) ) << " Input Data Type: " << QString::fromStdString( p_inputImageData->GetScalarTypeAsString() ) << " pixels are not be processed with Altumira";
        return errorCode;
    }

    return outcome::success();    // it is not a processing error if we do not have valid pointers
}

Result<void> AltumiraImageProcessing::SetAltumiraActivationKey( const int p_altumiraLicenseMethodIndex, const QString & activationKey )
{
    HCVIE handle = nullptr;

    // Now get the Device ID
    unsigned long deviceId;
    if( auto result2 = this->ExecuteAltumiraFunction( CVLMGetHostId( handle, p_altumiraLicenseMethodIndex, &deviceId ) ) )    // handle is not used by CVLMGetHostId
    {
        // Print result as 8 hex characters to make it compatible with data entry in ContextVision Customer Portal
        // printf("The unique id is: %08x\n", deviceId);
        qInfo() << "Altumira: The unique device ID for Altumira is " << QString::number( deviceId );

        const char * productId = "0000313";
        const char ** possibleProductIds;
        if( auto result3 = this->ExecuteAltumiraFunction( CVLMGetPossibleModules( handle, &possibleProductIds ) ) )    // handle is not used by CVLMGetPossibleModules
        {
            int indexProductId = -1;
            for( int index = 0; possibleProductIds[index][0] != '\0'; index++ )
            {
                if( strcmp( possibleProductIds[index], productId ) == 0 )
                {
                    indexProductId = index;
                    qInfo() << "Altumira: Product ID was found among possible product IDs for Altumira";
                }
            }

            if( indexProductId != -1 )
            {
                // Check if there is already a license for the one and only Product ID with the index found
                if( this->ExecuteAltumiraFunction( CVLMCheckLicense( handle, indexProductId ) ) )    // handle is not used by CVLMCheckLicense
                {
                    qInfo() << "Altumira: The activation key for Altumira is already installed";
                }
                else
                {
                    if( !activationKey.isEmpty() )
                    {
                        auto imageProcessingActivationKeyArray = activationKey.toLocal8Bit();
                        auto imageProcessingActivationKey = imageProcessingActivationKeyArray.data();
                        if( auto result4 = this->ExecuteAltumiraFunction( CVLMSetKey( handle, indexProductId, imageProcessingActivationKey ) ) )    // handle is not used by CVLMSetKey
                        {
                            qInfo() << "Altumira: The activation key " << imageProcessingActivationKeyArray << " for Altumira was successfully installed";
                        }
                        else
                        {
                            std::string additionalInformation = "Could not install the activation key " + imageProcessingActivationKeyArray.toStdString() + " for Altumira";
                            qCritical() << QString::fromStdString( PrintErrorCode( result4.error(), additionalInformation ) );
                            return result4;
                        }
                    }
                    else
                    {
                        auto errorCode = make_error_code( AltumiraErrorCode::LicenseError );
                        qCritical() << QString::fromStdString( PrintErrorCode( errorCode, "Please configure an activation key matching the usb dongle for Altumira processing" ) );
                        return errorCode;
                    }
                }
            }
            else
            {
                auto errorCode = make_error_code( AltumiraErrorCode::LicenseError );
                qWarning() << "Could not find the product ID in the list of possible products IDs for Altumira";
                return errorCode;
            }
        }
        else
        {
            qWarning() << QString::fromStdString( PrintErrorCode( result3.error(), "Could not retrieve the list of possible product IDs for Altumira" ) );
            return result3;
        }
    }
    else
    {
        qWarning() << QString::fromStdString( PrintErrorCode( result2.error(), "Could not retrieve the unique device ID for Altumira" ) );
        return result2;
    }

    return outcome::success();
}
