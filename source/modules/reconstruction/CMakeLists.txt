if(TOMO_ENABLE_PROJECTOR_TEST)									
	add_library(Projector	STATIC	Projector.cu 
									Projector.h			
									CudaErrorHandler.h									
									)

	target_link_libraries( Projector 		TomoGeometry
											ErrorHandling
											VTK::CommonCore
											VTK::IOImage
											VTK::CommonDataModel
											)
			
	add_library( Reconstructors		Reconstructors.h
									ReconstructorsErrorCode.cpp
									ReconstructorsErrorCode.h
									)


	target_link_libraries( Reconstructors 		Projector
												TomoGeometry
												PhantomMaker
												ErrorHandling
												VTK::CommonCore
												VTK::IOImage
												VTK::CommonDataModel
												VTK::ImagingMath
												VTK::FiltersCore
												)
	  
endif()