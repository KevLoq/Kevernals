								  
# Library for processing images
if(TOMO_ENABLE_PHANTOM_MAKER_TEST OR TOMO_ENABLE_PROJECTOR_TEST OR TOMO_ENABLE_RECONSTRUCTION_TEST)
	add_library( TomoGeometry  			TomoGeometry.h
										TomoGeometry.cpp
										TomoVolume.h
										TomoVolume.cpp
										TomoProjectionsSet.h
										TomoProjectionsSet.cpp
										TomoTable.h
										TomoTable.cpp
										TomoGeometryErrorCode.h
										TomoGeometryErrorCode.cpp
										)
															
	target_link_libraries( TomoGeometry		Qt5::Core
											Qt5::Xml
											ErrorHandling )
											
	add_library( RayProjectionGeometry 		VolumeData.h
											VolumeData.cpp )
										
	target_link_libraries( RayProjectionGeometry		Qt5::Core
														Qt5::Xml
														ErrorHandling )
endif()

if(TOMO_ENABLE_PROJECTOR_TEST)	
	add_library( RadonMatrix 		RadonMatrixManager.h
									RadonMatrixManager.cpp
									RadonMatrixErrorCode.h
									RadonMatrixErrorCode.cpp )

															
	target_link_libraries( RadonMatrix		Qt5::Core
											TomoGeometry 
											ErrorHandling 
											)
										
endif()	
