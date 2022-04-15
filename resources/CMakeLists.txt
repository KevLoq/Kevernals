#-----------------------------------------------------------------------------------------------------------------------
# Copy resources directory
#-----------------------------------------------------------------------------------------------------------------------

set( RESOURCE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
file( TO_CMAKE_PATH ${PROJECT_BINARY_DIR} KEVERNALS_BUILD_PATH )  # remove backslashes
set( RESOURCE_INSTALL_DIR "${KEVERNALS_BUILD_PATH}" )


configure_file( ${RESOURCE_SOURCE_DIR}/CopyResources.cmake.in ${RESOURCE_INSTALL_DIR}/CopyResources.cmake @ONLY )

# The target below is always considered out of date by CMake
# CMake is used in process script mode to execute the script CopyResources.cmake
add_custom_target( CopyResources
                   COMMAND ${CMAKE_COMMAND} -P ${RESOURCE_INSTALL_DIR}/CopyResources.cmake
                   WORKING_DIRECTORY ${RESOURCE_SOURCE_DIR}
                   COMMENT "Copying resource directory" )
