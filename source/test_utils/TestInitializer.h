#pragma once

#include <gtest/gtest.h>

using ::testing::InitGoogleTest;
 
inline int LaunchTest( int p_argc, char ** p_argv )
{
    ::testing::InitGoogleTest( &p_argc, p_argv ); 
    // True if we have to set up the tests
    for( int i = 0; i < p_argc; i++ )
    {
        // If Google Test Adapter only queries the list of tests (so doesn't execute the tests)
        if( std::string( p_argv[i] ) == "--gtest_list_tests" )
        {
            return EXIT_SUCCESS;
        }
    }
    return RUN_ALL_TESTS();
}

