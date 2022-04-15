
if(TOMO_ENABLE_PHANTOM_MAKER_TEST OR TOMO_ENABLE_PROJECTOR_TEST)								
	add_library( PhantomMaker 	PhantomMaker.cpp
								PhantomMaker.h )

	target_link_libraries( PhantomMaker		Qt5::Core
											VTK::CommonCore
											VTK::IOImage
											VTK::ImagingMath
											VTK::CommonDataModel
											TomoGeometry
											)
endif()
	add_library( DICOMReader 	DICOMReader.cpp
								DICOMReader.h
								DICOMReaderErrorCode.cpp 
								DICOMReaderErrorCode.h
								)

	target_link_libraries( DICOMReader		Qt5::Core
											VTK::CommonCore
											VTK::DICOM
											TomoGeometry
											) 