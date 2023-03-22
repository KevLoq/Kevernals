 if( TOMO_ENABLE_PROJECTOR_TEST )
	add_executable( Reconstuct	 	mainReconstruct.cpp )

	set_property(TARGET Reconstuct PROPERTY CUDA_SEPARABLE_COMPILATION ON)
	
	target_link_libraries( Reconstuct		DICOMReader
											PhantomMaker
											TomoGeometry
											Projector
											Reconstructors
											)

	# To trigger the copy of resources when compiling TomosynthesisTest
	add_dependencies( Reconstuct CopyResources )
endif()


if( TOMO_ENABLE_PHANTOM_MAKER_TEST )
	add_executable( PhantomMakerTest 	mainPhantomMakerTest.cpp )

	set_property(TARGET PhantomMakerTest PROPERTY CUDA_SEPARABLE_COMPILATION ON) 
			
	target_link_libraries( PhantomMakerTest		VTK::CommonCore
												PhantomMaker
												TomoGeometry
												)
										
endif()
