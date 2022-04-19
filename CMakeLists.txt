cmake_minimum_required(VERSION 3.22)

# set the project name
project(KEVERNALS CUDA CXX)

# Add the possibility to organize projects by folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option( ENABLE_ADDRESS_SANITIZER "Enable Asan Address sanitizer" OFF )
option( TOMO_ENABLE_PHANTOM_MAKER_TEST "Tomography: enable Phantom Generation tool testing" ON )
option( TOMO_ENABLE_PROJECTOR_TEST "Tomography: enable Projector tool testing" ON )
option( ML_ENABLE_TRIAL "Machine Learning: enable machine learning trial" ON )
option( ENABLE_TOY_TEST "Enable toy testing" OFF )

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#see https://stackoverflow.com/questions/40739061/error-lnk1104-cannot-open-file-debug-myprojectlib-lib

# Specifies the C++ standard whose features are requested to build all target
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CUDA_STANDARD_REQUIRED true)

message( STATUS "CMAKE_BUILD_DIRECTORY ${CMAKE_BUILD_DIRECTORY}" )
message( STATUS "OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}" )
message( STATUS "PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}" )

# Vtk lib finding
message( STATUS "VTK_DIR ${VTK_DIR}" )
message( STATUS "Torch_DIR ${Torch_DIR}" ) 
message( STATUS "GDCM_DIR ${GDCM_DIR}" ) 
message( STATUS "Qt5_DIR ${Qt5_DIR}" )
set(CMAKE_PREFIX_PATH ${Torch_DIR} ${Qt5_DIR} ${CMAKE_MODULE_PATH})


# Set the name of the folders for *_autogen targets and source files
# that are added automatically by CMake for targets for which AUTOMOC is enabled
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER AutomocTargets)
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP AutomocSources)

# Handle the Qt moc preprocessor automatically (signal/slot mechanism)
set(CMAKE_AUTOMOC ON)

# Handle the Qt rcc preprocessor automatically (resources)
set(CMAKE_AUTORCC ON)

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


# Find Altumira library
# Note: an imported target is created: Altumira::altumira
find_package( AltumiraPlus )
if( AltumiraPlus_FOUND )#
	add_definitions( -DALTUMIRA_PLUS )
endif()
	
# Find Qt5 libraries
# Note: imported target names should be preferred
# That is, you should use Qt5::<module> instead of Qt5<Module>_LIBRARIES in CMake commands such as target_link_libraries()
set( QT_MIN_VERSION 5.15.8 )
find_package( Qt5 COMPONENTS Core Xml REQUIRED CONFIG )



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
									RenderingContextOpenGL2
									RenderingCore
									RenderingFreeType
									RenderingGL2PSOpenGL2
									RenderingOpenGL2
									)
if(VTK_FOUND)
	#include_directories( ${VTK_INCLUDE_DIRS} )
    message( STATUS "Found VTK ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}" )	
	message( STATUS "VTK_INCLUDE_DIRS ${VTK_INCLUDE_DIRS}" )
endif()
 
find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    message( STATUS "Found CUDA Toolkit ${CUDAToolkit_VERSION_MAJOR}.${CUDAToolkit_VERSION_MINOR}" )
endif()

find_package(Torch REQUIRED)
if(TORCH_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
    message( STATUS "Found Torch ${TORCH_VERSION}" )
endif()

add_subdirectory(resources)
		  
# Override add_library to automatically add the target in the folder it has been created!
function(add_library name)
    _add_library(${name} ${ARGN})
    kevernals_move_target_into_VS_folder_named_like_target_directory( ${name} )
endfunction()

# Include top level directory as an include directory so each header can be included relatively to this top level
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/source )
# Include top level binary directory as an include directory so export headers can be found relatively to this top level
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/source )

include_directories( ${CUDA_INCLUDE_DIRS} )
 

add_subdirectory(source)

