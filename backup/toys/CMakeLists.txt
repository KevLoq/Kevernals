# Library for processing images
add_library( ToyLib 	ToysGeometryUtils.h
						ToySet.h
						ToySet.cpp
						)

														
target_link_libraries( ToyLib	Qt5::Core )


add_library(ToyMatrixMulCUDA	STATIC ToyMatrixMul.cu
								ToyMatrixMul.h
								)

target_include_directories(ToyMatrixMulCUDA PUBLIC "C:/ProgramData/NVIDIA Corporation/CUDA Samples/v11.4/common/inc" )

	#set_property(TARGET ToyMatrixMulCUDA PROPERTY CUDA_SEPARABLE_COMPILATION ON)
	#set_property(TARGET ToyMatrixMulCUDA PROPERTY CUDA_USE_STATIC_CUDA_RUNTIME  OFF)


#link_libraries( ToyMatrixMulCUDA CUDA::cudart_static )	
				 	