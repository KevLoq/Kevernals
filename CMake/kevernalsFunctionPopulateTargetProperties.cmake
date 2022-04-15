#-----------------------------------------------------------------------------------------------------------------------
# Import a library as a target
# Calling this function adds libraries location to an import target. This function is meant to be used by custom Find_Something.cmake
# Usage:
#   kevernals_populate_target_properties( target configuration dllLocation libLocation )
#-----------------------------------------------------------------------------------------------------------------------

macro( kevernals_populate_target_properties Target Configuration LIB_LOCATION IMPLIB_LOCATION )

    set_property( TARGET ${Target} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration} )

    if( NOT "${LIB_LOCATION}" STREQUAL "" )
        set_target_properties( ${Target} PROPERTIES IMPORTED_LOCATION_${Configuration} ${LIB_LOCATION} )
    endif()

    if( NOT "${IMPLIB_LOCATION}" STREQUAL "" )
        set_target_properties( ${Target} PROPERTIES IMPORTED_IMPLIB_${Configuration} ${IMPLIB_LOCATION} )
    endif()

endmacro()

