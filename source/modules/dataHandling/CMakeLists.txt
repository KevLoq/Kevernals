
if(TOMO_ENABLE_PHANTOM_MAKER_TEST OR TOMO_ENABLE_PROJECTOR_TEST)								
	add_library( PhantomMaker 	PhantomMaker.cpp
								PhantomMaker.h )

	target_link_libraries( PhantomMaker		VTK::CommonCore
											VTK::IOImage
											VTK::ImagingMath
											VTK::CommonDataModel
											BasicGeometry
											TomoGeometry
											)
endif()
	add_library( DICOMReader 	DICOMReader.cpp
								DICOMReader.h
								DICOMReaderErrorCode.cpp 
								DICOMReaderErrorCode.h
								)

	target_link_libraries( DICOMReader		VTK::CommonCore
											VTK::DICOM
											VTK::FiltersCore
											TomoGeometry
											) 