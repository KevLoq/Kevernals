if(TOMO_ENABLE_RECONSTRUCTION_TEST)
	add_library(Projection 		IProjector.h
								IProjector.cpp
								ProjectorErrorCode.h
								ProjectorErrorCode.cpp
								RayTracingProjector.h
								RayTracingProjector.cpp
								ShiftAndAddProjector.h
								ShiftAndAddProjector.cpp
								)


	target_link_libraries( Projection 		Qt5::Core
											TomoGeometry
											ErrorHandling
											VTK::CommonCore
											VTK::IOImage
											VTK::CommonDataModel
											VTK::ImagingMath
											VTK::FiltersCore
											VTK::DICOM
											)
endif()