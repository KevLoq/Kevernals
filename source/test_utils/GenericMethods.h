#pragma once

#include <random>
#include <string>
#include <iostream>

template<typename T>
T Random()
{
    // Seed with a real random value, if available 
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(static_cast<double>(INT_MIN), static_cast<double>(INT_MAX));
    // Choose a random float value between INT_MIN and INT_MAX
     return static_cast<T>(dis(gen));
}

template<typename T>
std::string ToStdString(T p_value)
{  
    // Create an output string stream
    std::ostringstream streamObj;
    // Set Fixed -Point Notation
    streamObj << std::fixed;
    // Set precision to 20 digits for being safe
    streamObj << std::setprecision(20);
    //Add double to stream
    streamObj << p_value;
    return streamObj.str();
}