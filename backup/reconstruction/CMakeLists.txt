
if(TOMO_ENABLE_RECONSTRUCTION_TEST)
	add_library(Reconstruction 	ITomosynthesisReconstructor.h
								ITomosynthesisReconstructor.cpp 
								TomosynthesisReconstructorErrorCode.h
								TomosynthesisReconstructorErrorCode.cpp
								TomosynthesisARTReconstructor.h
								TomosynthesisARTReconstructor.cpp
								TomosynthesisFBPReconstructor.h
								TomosynthesisFBPReconstructor.cpp
								)


	target_link_libraries( Reconstruction 	Qt5::Core
											TomoGeometry
											Projection
											RadonMatrix
											ErrorHandling
											VTK::CommonCore
											VTK::IOImage
											VTK::CommonDataModel
											VTK::ImagingMath
											VTK::FiltersCore
											VTK::DICOM
											)
endif()
if(TOMO_ENABLE_PROJECTOR_TEST)									
	add_library(Projector	STATIC	Projector.cu 
									Projector.h									
									)
 
	target_include_directories(Projector PUBLIC "C:/ProgramData/NVIDIA Corporation/CUDA Samples/v11.4/common/inc" )

	target_link_libraries( Projector 		Qt5::Core
											TomoGeometry
											ErrorHandling
											RadonMatrix 
											VTK::CommonCore
											VTK::IOImage
											VTK::CommonDataModel
											)
	  
endif()