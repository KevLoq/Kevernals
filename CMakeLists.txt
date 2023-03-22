cmake_minimum_required(VERSION 3.22)

# set the project name
project(KEVERNALS CUDA CXX)

# Add the possibility to organize projects by folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option( ENABLE_ADDRESS_SANITIZER "Enable Asan Address sanitizer" OFF )
option( TOMO_ENABLE_PHANTOM_MAKER_TEST "Tomography: enable Phantom Generation tool testing" ON )
option( TOMO_ENABLE_PROJECTOR_TEST "Tomography: enable Projector tool testing" ON )


set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#see https://stackoverflow.com/questions/40739061/error-lnk1104-cannot-open-file-debug-myprojectlib-lib

# Specifies the C++ standard whose features are requested to build all target
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CUDA_STANDARD_REQUIRED true)
  

set(CMAKE_PREFIX_PATH ${CMAKE_MODULE_PATH})


if( ENABLE_ADDRESS_SANITIZER )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address" )
endif()

#-----------------------------------------------------------------------------------------------------------------------
# Update CMake module path
#-----------------------------------------------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

# Include custom cmake functions
include( kevernalsFunctionPopulateTargetProperties )
include( kevernalsFunctionMoveTargetToVSFolder )
include( kevernalsFunctionAddHeaderOnly )
  
# For all targets, these properties specify the directories into which runtime target files should be built
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/RelWithDebInfo)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/RelWithDebInfo)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/RelWithDebInfo)   # the corresponding import library of a dll is treated as an archive target on windows

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/Debug)   # the corresponding import library of a dll is treated as an archive target on windows


# Find VTK libraries
# COMPONENTS vtkRenderingExternal vtkRenderingImage vtkIOImage vtkImagingStencil
find_package( VTK 9.0.1   REQUIRED 	COMPONENTS 
									CommonCore 
									CommonColor 
									CommonDataModel 
									IOImage 
									DICOM 
									ImagingMath 
									FiltersCore 
									ImagingCore 
									ImagingFourier 
									ImagingMath 
									InteractionStyle
									InteractionImage
									RenderingContextOpenGL2
									RenderingCore
									RenderingFreeType
									RenderingGL2PSOpenGL2
									RenderingOpenGL2
									)
if(VTK_FOUND) 
    message( STATUS "Found VTK ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}" )	
	message( STATUS "VTK_INCLUDE_DIRS ${VTK_INCLUDE_DIRS}" )
endif()
 
find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    message( STATUS "Found CUDA Toolkit ${CUDAToolkit_VERSION_MAJOR}.${CUDAToolkit_VERSION_MINOR}" )
endif()


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
 
add_subdirectory(resources)
		  
# Override add_library to automatically add the target in the folder it has been created!
function(add_library name)
    _add_library(${name} ${ARGN})
    kevernals_move_target_into_VS_folder_named_like_target_directory( ${name} )
endfunction()

include(kevernalsFunctionAddTest)

# Include top level directory as an include directory so each header can be included relatively to this top level
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/source )
# Include top level binary directory as an include directory so export headers can be found relatively to this top level
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/source )

include_directories( ${CUDA_INCLUDE_DIRS} )
 

add_subdirectory(source)

