#-----------------------------------------------------------------------------------------------------------------------
# Add a header-only library
#
# Calling this function creates a custom target with no build rule with the only purpose to show header files in IDEs.
#
# Usage:
#
#   tomoAddHeaderOnly( targetName header1.h header2.h ... )
#-----------------------------------------------------------------------------------------------------------------------

function( kevernalsAddHeaderOnly targetName headerName )

    # Put optional arguments m in the variable optional_arg
    set( optional_headers ${ARGN} )

    add_custom_target( ${targetName} SOURCES ${headerName}
                                             ${optional_headers} )

    kevernals_move_target_into_VS_folder_named_like_target_directory( ${targetName} )

endfunction()
