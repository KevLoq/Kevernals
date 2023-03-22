								  
# Library for processing images
if(TOMO_ENABLE_PHANTOM_MAKER_TEST OR TOMO_ENABLE_PROJECTOR_TEST OR TOMO_ENABLE_RECONSTRUCTION_TEST)
	add_library( TomoGeometry  			PixelOnProjection.h
										TomoGeometry.h
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

															
	target_link_libraries( TomoGeometry		ErrorHandling
								  			TinyXML
											BasicGeometry )
endif()

add_library( BasicGeometry  	Dim3.h
								Dim3Vectorial.h
								Dim2.h
								Dim2Vectorial.h
								Voxel.h
								Voxel.cpp
								Position3D.h
								Position3D.cpp
								WSize3D.h
								WSize3D.cpp
								Size3D.h
								Size3D.cpp
								VoxelSpacing.h
								VoxelSpacing.cpp
								Vector3D.h
								Vector3D.cpp
								Pixel.h
								Pixel.cpp
								Position2D.h
								Position2D.cpp
								WSize2D.h
								WSize2D.cpp
								Size2D.h
								Size2D.cpp
								PixelSpacing.h
								PixelSpacing.cpp
								Vector2D.h
								Vector2D.cpp
								Cylinder.h
								Pave.h
								Sphere.h
								Roi2D.h
								Roi3D.h
								)
 

kevernals_add_test_file( Dim2_test BasicGeometry ) 
kevernals_add_test_file( Dim3_test BasicGeometry ) 
kevernals_add_test_file( Dim3Vectorial_test BasicGeometry ) 
kevernals_add_test_file( Dim2Vectorial_test BasicGeometry )  