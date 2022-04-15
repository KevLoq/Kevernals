# Library for processing images
add_library( AltumiraImageProcessing STATIC	AltumiraImageProcessing.cpp
											AltumiraImageProcessing.h
											AltumiraParameters.h
											AltumiraErrorCode.cpp
											AltumiraErrorCode.h)

														
target_link_libraries( AltumiraImageProcessing	Altumira::altumira
												ErrorHandling
												Qt5::Core
												VTK::CommonDataModel  )

