#pragma once

#include <sstream>
#include <string>
#include <system_error>


// Note: do not put LOG( ERROR ) directly in the implementation below as it would print the name of this file instead
// of the name of the file and the line number where the error occurs
inline std::string PrintErrorCode( const std::error_code & p_errorCode,
                                   const std::string & p_additionalComment = std::string( "none." ) )
{
    // We want to print the error the following way: 'categoryName:xx error message. Details: additional comment'
    // Note: std::error_code prints to categoryName:xx where xx is the number of the error (value of the associated enum)
    std::stringstream stream;
    stream << p_errorCode << " " << p_errorCode.message() << ". Details: " << p_additionalComment;
    return stream.str();
}
