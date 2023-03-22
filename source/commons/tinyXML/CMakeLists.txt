# As std::error_category instances must be globally unique in a process as per the C++ standard,
# the definition of a custom error_category in a shared library is the only standards conforming one.
add_library( TinyXML SHARED 	tinyxml2.cpp
                                tinyxml2.h )