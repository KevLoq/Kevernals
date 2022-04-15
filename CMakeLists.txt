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


#set(KL_DEV_DIR $ENV{KL_DEV_DIR})
#set(ADAM_SRC_DIR $ENV{ADAM_SRC_DIR})
#set(TOMO_SRC_DIR $ENV{TOMO_SRC_DIR})
#set(TOMO_EXTERNAL_DIR $ENV{TOMO_EXTERNAL_DIR})
#set(TOMO_EXTERNAL_LIB_DIR $ENV{TOMO_EXTERNAL_LIB_DIR})
#set(KEVERNALS_BUILD_DIR $ENV{KEVERNALS_BUILD_DIR})
#set(DICOMCONNECT_ROOT $ENV{DICOMCONNECT_ROOT})
#set(Qt5_DIR $ENV{Qt5_DIR})
#set(VTK_DIR $ENV{VTK_DIR}) 
#set(GDCM_DIR $ENV{GDCM_DIR})
#set(EIGEN3_DIR $ENV{EIGEN3_DIR})
#set(ALTUMIRA_PLUS_ROOT $ENV{ALTUMIRA_PLUS_ROOT})
#set(GIT_ROOT $ENV{GIT_ROOT})
#set(Torch_DIR $ENV{Torch_DIR})
#set(BOOST_INCLUDEDIR $ENV{BOOST_INCLUDEDIR})

# Torch lib finding
set(VTK_DIR ${EXTERNALS_DIR}/binaries/VTK-9.0.1)
message( STATUS "VTK_DIR ${VTK_DIR}" )

# Torch lib finding
set(Torch_DIR ${EXTERNALS_DIR}/binaries/torch/libtorch-win-shared-with-deps-1.11.0+cu113/libtorch)
message( STATUS "Torch_DIR ${Torch_DIR}" ) 

set(GDCM_DIR ${EXTERNALS_DIR}/binaries/gdcm-3.0.8)
message( STATUS "GDCM_DIR ${GDCM_DIR}" ) 

set(Qt5_DIR ${ADAM_EXTERNALS_DIR}/Qt-5.15.8/5.15.8/msvc2019_64)
message( STATUS "Qt5_DIR ${Qt5_DIR}" )
set(CMAKE_PREFIX_PATH ${Torch_DIR} ${Qt5_DIR} ${CMAKE_MODULE_PATH})

set(ALTUMIRA_PLUS_ROOT ${ADAM_EXTERNALS_DIR}/AltumiraPlus-5.3.4.16)

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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIRECTORY}/RelWithDebInfo)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${OUTPUT_DIRECTORY}/RelWithDebInfo)   # the corresponding import library of a dll is treated as an archive target on windows

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY}/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIRECTORY}/Debug)   # the corresponding import library of a dll is treated as an archive target on windows


# Find Altumira library
# Note: an imported target is created: Altumira::altumira
find_package( AltumiraPlus REQUIRED )
if( AltumiraPlus_FOUND )#
	add_definitions( -DALTUMIRA_PLUS )
endif()
	
# Find DICOMconnect library
# Note: 3 imported targets are created: DicomConnect::basics, DicomConnect::dicomServices and DicomConnect::gdtServices
#find_package( DICOMconnect 3.3.3 REQUIRED )
#if(DICOMCONNECT_FOUND)#
#	add_definitions( -DDICOMCONNECT )
#    message( STATUS "Found DICOMconnect" )
#endif()

# Find Qt5 libraries
# Note: imported target names should be preferred
# That is, you should use Qt5::<module> instead of Qt5<Module>_LIBRARIES in CMake commands such as target_link_libraries()
#set (Qt5_DIR C:/dev/ADAM-externals/Qt-5.15.2/5.15.2/msvc2019_64/lib/cmake/Qt5 )
set( QT_MIN_VERSION 5.15.8 )
find_package( Qt5 COMPONENTS Core Xml REQUIRED CONFIG )


# Disabling narrowing conversions in signal/slot connections
# https://www.kdab.com/disabling-narrowing-conversions-in-signal-slot-connections/
#if(Qt5Core_FOUND)
#    add_definitions( -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT )
#    message( STATUS "Found Qt ${Qt5Core_VERSION}" )
#endif()

# Find Boost library (used for your custom sink based on g3log-rotate)
# Use imported target Boost::boost in target_link_libraries whenever a target requires Boost
#find_package( Boost 1.78 REQUIRED )
#if(Boost_FOUND)
	# Disable autolinking in Boost since we have to do it manually using target_link_libraries
	# to support other platforms
#	add_definitions( -DBOOST_ALL_NO_LIB )

	# To fix the error occuring with Boost
	# "Please define _WIN32_WINNT or _WIN32_WINDOWS appropriately. Assuming _WIN32_WINNT=0x0501 (i.e. Windows XP target)"
	# Instead, we choose to target Windows 10
#	add_definitions( -D_WIN32_WINNT=0x0A00 )

	# Remove warning C4996: 'std::allocator<void>': warning STL4009: std::allocator<void> is deprecated in C++17
#	add_definitions( -D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING )

#endif()


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
    message( STATUS "Found VTK ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}" )
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

#include_directories( ${EIGEN3_DIR} )


add_subdirectory(source)

