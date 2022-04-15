# Locate the Altumira Plus library
#
# Defines the following variable:
#
#   AltumiraPlus_FOUND - true if the Altumira Plus library is found
#
# Also defines the following imported targets:
#
#   Altumira::altumira
#
# Accepts the following variables as input:
#
#   ALTUMIRA_PLUS_ROOT (as an environment variable) - The root directory of the Altumira Plus install prefix
#
#-----------------------
# Example Usage:
#
#    find_package( AltumiraPlus REQUIRED )
#
#    add_executable( foo foo.cc )
#    target_link_libraries( foo Altumira::altumira )
#
#=============================================================================

# Altumira can be found by defining the environment variable ALTUMIRA_PLUS_ROOT
if( DEFINED ENV{ALTUMIRA_PLUS_ROOT} )

    set( ALTUMIRA_PLUS_ROOT $ENV{ALTUMIRA_PLUS_ROOT} )
    file( TO_CMAKE_PATH ${ALTUMIRA_PLUS_ROOT} ALTUMIRA_PLUS_ROOT )  # remove backslashes

else()
    message( STATUS "Please define the environment variable ALTUMIRA_PLUS_ROOT to find Altumira Plus" )
endif()

unset( ALTUMIRA_PLUS_INCLUDE_DIR CACHE )

# Find all include directories
find_path( ALTUMIRA_PLUS_INCLUDE_DIR cvie.h HINTS ${ALTUMIRA_PLUS_ROOT}/include )
mark_as_advanced( ALTUMIRA_PLUS_INCLUDE_DIR )

set( CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll" )

# Allow ALTUMIRA_PLUS_LIBRARY to be set manually, as the location of the Altumira Plus library
if( NOT ALTUMIRA_PLUS_LIBRARY )

    unset( ALTUMIRA_PLUS_DLL_RELEASE CACHE )
    unset( ALTUMIRA_PLUS_LIBRARY_RELEASE CACHE )

    find_library( ALTUMIRA_PLUS_DLL_RELEASE NAMES cvie64 HINTS ${ALTUMIRA_PLUS_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH )
    find_library( ALTUMIRA_PLUS_LIBRARY_RELEASE NAMES cvie64 HINTS ${ALTUMIRA_PLUS_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH )

    mark_as_advanced( ALTUMIRA_PLUS_DLL_RELEASE )
    mark_as_advanced( ALTUMIRA_PLUS_LIBRARY_RELEASE )

endif()

# Handle the QUIETLY and REQUIRED arguments and set ALTUMIRA_PLUS_FOUND to TRUE if all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( AltumiraPlus DEFAULT_MSG ALTUMIRA_PLUS_INCLUDE_DIR ALTUMIRA_PLUS_DLL_RELEASE ALTUMIRA_PLUS_LIBRARY_RELEASE )

if( AltumiraPlus_FOUND )
    message( STATUS "Found Altumira Plus")
endif()


# Let's create the import target Altumira::altumira
if( AltumiraPlus_FOUND AND NOT TARGET Altumira::altumira )

    add_library( Altumira::altumira SHARED IMPORTED )

    if( ALTUMIRA_PLUS_INCLUDE_DIR )
        set_target_properties( Altumira::altumira PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ALTUMIRA_PLUS_INCLUDE_DIR}" )
    endif()

    kevernals_populate_target_properties( Altumira::altumira RELEASE ${ALTUMIRA_PLUS_DLL_RELEASE} ${ALTUMIRA_PLUS_LIBRARY_RELEASE} )

endif()
