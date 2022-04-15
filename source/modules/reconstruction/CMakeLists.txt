if(TOMO_ENABLE_PROJECTOR_TEST)									
	add_library(Projector	STATIC	Projector.cu 
									Projector.h			
									CudaErrorHandler.h									
									)

	target_link_libraries( Projector 		Qt5::Core
											TomoGeometry
											ErrorHandling
											VTK::CommonCore
											VTK::IOImage
											VTK::CommonDataModel
											)
	  
endif()