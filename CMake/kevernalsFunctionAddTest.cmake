#-----------------------------------------------------------------------------------------------------------------------
# Add a unit test using gtest or/and gmock
#
# Calling this function generates an executable from the file fileName.cpp. It is automatically linked to
# gtest and gmock libraries. Additional libraries can be optionally added after the target name
#
# Usage:
#
#   adam_add_test_file(fileName library1 library2...)
#-----------------------------------------------------------------------------------------------------------------------

function( kevernals_add_test_file fileName )
 
	# Put optional arguments m in the variable optional_arg
	set( optional_arg ${ARGN} )

	# We first create an executable
	add_executable( ${fileName} ${fileName}.cpp )
	target_link_libraries( ${fileName} GTest::gtest
									   GTest::gtest_main
									   ${GMOCK_BOTH_LIBRARIES}
									   ${optional_arg} )

	# Move each test target in a 'tests' directory
	string( REPLACE "${PROJECT_SOURCE_DIR}/source/" "" REL_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
	set_target_properties( ${fileName} PROPERTIES FOLDER ${REL_DIR}/tests )

	# We add the test to the list of test maintained by CMake
	# http://stackoverflow.com/questions/21394768/cmake-cannot-find-test-if-cmake-runtime-output-directory-is-changed#21413672
	add_test( NAME ${fileName}
			  WORKING_DIRECTORY ${OUTPUT_DIRECTORY}
			  COMMAND $<TARGET_FILE:${fileName}> "--gtest_output=xml:./" )

	# Set a timeout of 120s
	set_tests_properties( ${fileName} PROPERTIES TIMEOUT 120 ) 

endfunction()
