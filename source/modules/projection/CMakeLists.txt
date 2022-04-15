add_library(Projection 		IProjector.h
							IProjector.cpp
							ProjectorErrorCode.h
							ProjectorErrorCode.cpp
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
