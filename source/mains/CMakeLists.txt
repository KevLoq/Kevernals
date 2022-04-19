 if( TOMO_ENABLE_PROJECTOR_TEST )
	add_executable( ProjectorTest	 	mainProjectorTest.cpp )

	set_property(TARGET ProjectorTest PROPERTY CUDA_SEPARABLE_COMPILATION ON)
	
	target_link_libraries( ProjectorTest		Qt5::Core  
												DICOMReader
												PhantomMaker
												TomoGeometry
												Projector
												)

	# To trigger the copy of resources when compiling TomosynthesisTest
	add_dependencies( ProjectorTest CopyResources )
endif()


if( TOMO_ENABLE_PHANTOM_MAKER_TEST )
	add_executable( PhantomMakerTest 	mainPhantomMakerTest.cpp )

	set_property(TARGET PhantomMakerTest PROPERTY CUDA_SEPARABLE_COMPILATION ON) 
			
	target_link_libraries( PhantomMakerTest		Qt5::Core  
												VTK::CommonCore
												PhantomMaker
												TomoGeometry
												)
										
endif()
	

if( TOMO_ENABLE_TOY_TEST )

	add_executable( ToysTest mainToysTest.cpp )
					 
	set_property(TARGET ToysTest PROPERTY CUDA_SEPARABLE_COMPILATION ON) 
			 
	target_link_libraries( ToysTest		Qt5::Core
										ToyMatrixMulCUDA
										) 
										 

endif()

if( ML_ENABLE_TRIAL )

	add_executable(MLTrial mainMachineLearningTrial.cpp)
	
	set_property(TARGET MLTrial PROPERTY CUDA_SEPARABLE_COMPILATION ON)
	
	target_link_libraries( MLTrial PUBLIC ${TORCH_LIBRARIES} Qt5::Core )
	
	if (MSVC)
	  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
	  add_custom_command(TARGET MLTrial
						 POST_BUILD
						 COMMAND ${CMAKE_COMMAND} -E copy_if_different
						 ${TORCH_DLLS}
						 $<TARGET_FILE_DIR:MLTrial>)
	endif (MSVC)
	
endif()

add_executable(SimplestProgram mainSimple.cpp) 

target_link_libraries( SimplestProgram VTK::CommonCore VTK::IOImage )
	
