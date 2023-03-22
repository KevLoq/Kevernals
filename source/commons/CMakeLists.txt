kevernalsAddHeaderOnly( ImageDataPtr ImageDataPtr.h)
kevernalsAddHeaderOnly( MagicEnum MagicEnum.h)
kevernalsAddHeaderOnly( Maths Maths.h)
kevernalsAddHeaderOnly( GlobalUtils GlobalUtils.h)
kevernalsAddHeaderOnly( HistogramMatchingTools HistogramMatchingTools.h)
kevernalsAddHeaderOnly( DataTypeValidator DataTypeValidator.h)

# As std::error_category instances must be globally unique in a process as per the C++ standard,
# the definition of a custom error_category in a shared library is the only standards conforming one.
add_library( ErrorHandling SHARED TomoErrorCondition.cpp
                                  TomoErrorCondition.h
								  PrintErrorCode.h )
								  
								  
add_subdirectory(tinyXML)