# Locate the DICOMconnect library
#
# Defines the following variable:
#
#   DICOMCONNECT_FOUND - true if the DICOMconnect library is found
#
# Also defines the following imported targets:
#
#   DicomConnect::basics
#   DicomConnect::dicomServices
#   DicomConnect::gdtServices
#
# Accepts the following variables as input:
#
#   DICOMCONNECT_ROOT (as an environment variable) - The root directory of the DICOMconnect install prefix
#
#-----------------------
# Example Usage:
#
#    find_package( DICOMCONNECT REQUIRED )
#
#    add_executable( foo foo.cc )
#    target_link_libraries( foo DicomConnect::basics )
#
#=============================================================================

set (DICOMCONNECT_ROOT C:/dev/ADAM-externals/DICOMconnect-2.5.4)
file( TO_CMAKE_PATH ${DICOMCONNECT_ROOT} DICOMCONNECT_ROOT )  # remove backslashes

unset( DICOMCONNECT_INCLUDE_DIR CACHE )
unset( DICOMCONNECT_BASICS_INCLUDE_DIR CACHE )
unset( DICOMCONNECT_DICOMLIB_INCLUDE_DIR CACHE )
unset( DICOMCONNECT_GDT_INCLUDE_DIR CACHE )
unset( DICOMCONNECT_LOG_INCLUDE_DIR CACHE )

# Find all include directories
find_path( DICOMCONNECT_INCLUDE_DIR Basics/BasicsModule.h HINTS ${DICOMCONNECT_ROOT}/include )
mark_as_advanced( DICOMCONNECT_INCLUDE_DIR )

find_path( DICOMCONNECT_BASICS_INCLUDE_DIR BasicsModule.h HINTS ${DICOMCONNECT_ROOT}/include/Basics )
mark_as_advanced( DICOMCONNECT_BASICS_INCLUDE_DIR )

find_path( DICOMCONNECT_DICOMLIB_INCLUDE_DIR BaseScu.h HINTS ${DICOMCONNECT_ROOT}/include/DicomLib )
mark_as_advanced( DICOMCONNECT_DICOMLIB_INCLUDE_DIR )

find_path( DICOMCONNECT_GDT_INCLUDE_DIR GdtConfigurationReader.h HINTS ${DICOMCONNECT_ROOT}/include/Gdt )
mark_as_advanced( DICOMCONNECT_GDT_INCLUDE_DIR )

find_path( DICOMCONNECT_LOG_INCLUDE_DIR appender.h HINTS ${DICOMCONNECT_ROOT}/include/log4cplus )
mark_as_advanced( DICOMCONNECT_LOG_INCLUDE_DIR )

# Create a variable containing the 4 include directories
set( DICOMCONNECT_INCLUDE_DIRS "${DICOMCONNECT_INCLUDE_DIR};${DICOMCONNECT_BASICS_INCLUDE_DIR};${DICOMCONNECT_DICOMLIB_INCLUDE_DIR};${DICOMCONNECT_GDT_INCLUDE_DIR};${DICOMCONNECT_LOG_INCLUDE_DIR}" )

#set( CMAKE_FIND_LIBRARY_PREFIXES "lib" )
set( CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll" )

# Allow DICOMCONNECT_LIBRARY to be set manually, as the location of the DICOMconnect library
if( NOT DICOMCONNECT_LIBRARY )

    unset( DICOMCONNECT_BASICS_DLL_RELEASE CACHE )
    unset( DICOMCONNECT_BASICS_LIBRARY_RELEASE CACHE )
    unset( DICOMCONNECT_BASICS_DLL_DEBUG CACHE )
    unset( DICOMCONNECT_BASICS_LIBRARY_DEBUG CACHE )

    unset( DICOMCONNECT_DICOMSERVICES_DLL_RELEASE CACHE )
    unset( DICOMCONNECT_DICOMSERVICES_LIBRARY_RELEASE CACHE )
    unset( DICOMCONNECT_DICOMSERVICES_DLL_DEBUG CACHE )
    unset( DICOMCONNECT_DICOMSERVICES_LIBRARY_DEBUG CACHE )

    unset( DICOMCONNECT_GDTSERVICES_DLL_RELEASE CACHE )
    unset( DICOMCONNECT_GDTSERVICES_LIBRARY_RELEASE CACHE )
    unset( DICOMCONNECT_GDTSERVICES_DLL_DEBUG CACHE )
    unset( DICOMCONNECT_GDTSERVICES_LIBRARY_DEBUG CACHE )

    find_library( DICOMCONNECT_BASICS_DLL_RELEASE NAMES Basics_Win_x64 HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_BASICS_LIBRARY_RELEASE NAMES Basics_Win_x64 HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_BASICS_DLL_DEBUG NAMES Basics_Win_x64d HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_BASICS_LIBRARY_DEBUG NAMES Basics_Win_x64d HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH )

    find_library( DICOMCONNECT_DICOMSERVICES_DLL_RELEASE NAMES DicomServices_Win_x64 HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_DICOMSERVICES_LIBRARY_RELEASE NAMES DicomServices_Win_x64 HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_DICOMSERVICES_DLL_DEBUG NAMES DicomServices_Win_x64d HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_DICOMSERVICES_LIBRARY_DEBUG NAMES DicomServices_Win_x64d HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH )

    find_library( DICOMCONNECT_GDTSERVICES_DLL_RELEASE NAMES GdtServices_win_x64 HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_GDTSERVICES_LIBRARY_RELEASE NAMES GdtServices_win_x64 HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_GDTSERVICES_DLL_DEBUG NAMES GdtServices_win_x64d HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH )
    find_library( DICOMCONNECT_GDTSERVICES_LIBRARY_DEBUG NAMES GdtServices_win_x64d HINTS ${DICOMCONNECT_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH )

    mark_as_advanced( DICOMCONNECT_BASICS_DLL_RELEASE )
    mark_as_advanced( DICOMCONNECT_BASICS_DLL_DEBUG )
    mark_as_advanced( DICOMCONNECT_DICOMSERVICES_DLL_RELEASE )
    mark_as_advanced( DICOMCONNECT_DICOMSERVICES_DLL_DEBUG )
    mark_as_advanced( DICOMCONNECT_GDTSERVICES_DLL_RELEASE )
    mark_as_advanced( DICOMCONNECT_GDTSERVICES_DLL_DEBUG )
    
    # This macro takes a library base name as an argument, and will choose good values for basename_LIBRARY, basename_LIBRARIES,
    # basename_LIBRARY_DEBUG, and basename_LIBRARY_RELEASE depending on what has been found and set
    include( SelectLibraryConfigurations )
    select_library_configurations( DICOMCONNECT_BASICS )
    select_library_configurations( DICOMCONNECT_DICOMSERVICES )
    select_library_configurations( DICOMCONNECT_GDTSERVICES )

    # Create variables for each configuration containing the 3 libraries
    set( DICOMCONNECT_LIBRARY "${DICOMCONNECT_BASICS_LIBRARY};${DICOMCONNECT_DICOMSERVICES_LIBRARY};${DICOMCONNECT_GDTSERVICES_LIBRARY}" )
    set( DICOMCONNECT_LIBRARY_RELEASE "${DICOMCONNECT_BASICS_LIBRARY_RELEASE};${DICOMCONNECT_DICOMSERVICES_LIBRARY_RELEASE};${DICOMCONNECT_GDTSERVICES_LIBRARY_RELEASE}" )
    set( DICOMCONNECT_LIBRARY_DEBUG "${DICOMCONNECT_BASICS_LIBRARY_DEBUG};${DICOMCONNECT_DICOMSERVICES_LIBRARY_DEBUG};${DICOMCONNECT_GDTSERVICES_LIBRARY_DEBUG}" )

endif()

# Let's read DICOMconnect version from the versioned path the superbuild writes into dicom_config.xml
if( EXISTS "${DICOMCONNECT_ROOT}/config/dicom_config.xml")
    file( STRINGS "${DICOMCONNECT_ROOT}/config/dicom_config.xml" CONFIG_XML REGEX "<OpRes_Explanation_Dir>(.*)</OpRes_Explanation_Dir>" )
    string( REGEX MATCH "[0-9].[0-9].[0-9]" DICOMCONNECT_VERSION "${CONFIG_XML}" )
endif()

# Handle the QUIETLY and REQUIRED arguments and set DICOMCONNECT_FOUND to TRUE if all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( DICOMconnect DEFAULT_MSG DICOMCONNECT_INCLUDE_DIRS DICOMCONNECT_LIBRARY DICOMCONNECT_LIBRARY_RELEASE DICOMCONNECT_LIBRARY_DEBUG )


if( DICOMCONNECT_FOUND )
    message( STATUS "Found DICOMConnect ${DICOMCONNECT_VERSION}")
endif()

# Include custom function adam_populate_target_properties
include( kevernalsFunctionPopulateTargetProperties )

# Let's create the import target DicomConnect::basics
if( DICOMCONNECT_FOUND AND NOT TARGET DicomConnect::basics )

    add_library( DicomConnect::basics SHARED IMPORTED )

    if( DICOMCONNECT_INCLUDE_DIRS )
        set_target_properties( DicomConnect::basics PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${DICOMCONNECT_INCLUDE_DIRS}" )
    endif()

    kevernals_populate_target_properties( DicomConnect::basics RELEASE ${DICOMCONNECT_BASICS_DLL_RELEASE} ${DICOMCONNECT_BASICS_LIBRARY_RELEASE} )
    kevernals_populate_target_properties( DicomConnect::basics DEBUG ${DICOMCONNECT_BASICS_DLL_DEBUG} ${DICOMCONNECT_BASICS_LIBRARY_DEBUG} )

endif()

# Let's create the import target DicomConnect::dicomServices
if( DICOMCONNECT_FOUND AND NOT TARGET DicomConnect::dicomServices )

    add_library( DicomConnect::dicomServices SHARED IMPORTED )

    if( DICOMCONNECT_INCLUDE_DIRS )
        set_target_properties( DicomConnect::dicomServices PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${DICOMCONNECT_INCLUDE_DIRS}" )
    endif()

    # DicomServices depends on DicomConnect's basics
    set_target_properties( DicomConnect::dicomServices PROPERTIES INTERFACE_LINK_LIBRARIES "DicomConnect::basics" )

    kevernals_populate_target_properties( DicomConnect::dicomServices RELEASE ${DICOMCONNECT_DICOMSERVICES_DLL_RELEASE} ${DICOMCONNECT_DICOMSERVICES_LIBRARY_RELEASE} )
    kevernals_populate_target_properties( DicomConnect::dicomServices DEBUG ${DICOMCONNECT_DICOMSERVICES_DLL_DEBUG} ${DICOMCONNECT_DICOMSERVICES_LIBRARY_DEBUG} )

endif()

# Let's create the import target DicomConnect::gdtServices
if( DICOMCONNECT_FOUND AND NOT TARGET DicomConnect::gdtServices )

    add_library( DicomConnect::gdtServices SHARED IMPORTED )

    if( DICOMCONNECT_INCLUDE_DIRS )
        set_target_properties( DicomConnect::gdtServices PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${DICOMCONNECT_INCLUDE_DIRS}/Basics" )
    endif()

    kevernals_populate_target_properties( DicomConnect::gdtServices RELEASE ${DICOMCONNECT_GDTSERVICES_DLL_RELEASE} ${DICOMCONNECT_GDTSERVICES_LIBRARY_RELEASE} )
    kevernals_populate_target_properties( DicomConnect::gdtServices DEBUG ${DICOMCONNECT_GDTSERVICES_DLL_DEBUG} ${DICOMCONNECT_GDTSERVICES_LIBRARY_DEBUG} )

endif()
