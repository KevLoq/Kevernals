#-----------------------------------------------------------------------------------------------------------------------
# Set the directory of the target for Visual Studio
#
# Calling this function will add the target to the current directory in Visual Studio
#
# Usage:
#
#   adam_move_target_into_VS_folder_named_like_target_directory( target )
#-----------------------------------------------------------------------------------------------------------------------

#move_target_into_VS_directory( target )
#move_to_current_folder_in_VS( target )
#set_VS_folder_as_current_directory_for( target )
#set_target_directory_as_VS_folder
#name_VS_folder_like_target_directory
#adam_move_target_into_VS_folder_named_like_target_directory

function( kevernals_move_target_into_VS_folder_named_like_target_directory target )

    # Retrieve the type of target because moving the target to a folder is not possible for INTERFACE targets
    get_target_property(target_type ${target} TYPE)
    if ( NOT target_type STREQUAL "INTERFACE_LIBRARY")
        # Check that we are not in the source folder or the string will be "${PROJECT_SOURCE_DIR}/source" with absolute path
        # CMake will then try to make a folder with the HDD letter, which make Visual Studio bug
        if( NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}" )
            string( REPLACE "${PROJECT_SOURCE_DIR}/" "" REL_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
            set_target_properties( ${target} PROPERTIES FOLDER ${REL_DIR} )
        endif()
    endif() 

endfunction()
