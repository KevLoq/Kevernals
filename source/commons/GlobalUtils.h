#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <array>
#include <optional>

namespace glob {
    constexpr auto TWO = 2;
    constexpr auto THREE = 3;

    inline std::vector<std::string> SentenceToWords(const std::string & p_sentence)
    {
        std::istringstream inputStream(p_sentence);
        std::vector<std::string> words;
        do
        { 
            std::string substring;
            inputStream >> substring;
            if( !substring.empty() )
            {
                words.push_back(substring);
            }
        } while (inputStream);
        return words;
    }
    
    inline std::optional<std::array<std::string,TWO>> SentenceToArray2_OfWords(const std::string & p_sentence)
    {
        auto words = SentenceToWords(p_sentence);
        if(std::size(words) != TWO)
        {
            return std::nullopt;
        }
        return std::array<std::string,TWO>{words[0],words[1]};
    }
    
    inline std::optional<std::array<std::string,THREE>> SentenceToArray3_OfWords(const std::string & p_sentence)
    {
        auto words = SentenceToWords(p_sentence);
        if(std::size(words) != THREE)
        {
            return std::nullopt;
        }
        return std::array<std::string,THREE>{words[0],words[1],words[2]};
    }

    inline void WaitForKeyTyping()
    {
        do
        {
            std::cout << std::endl
                    << "Gotcha Press any key to continue...";
        } while( std::cin.get() != '\n' );
    }
};

template<typename T> inline std::ostream& operator<<( std::ostream& p_outputStream, std::vector<T> const & p_data) 
{ 
    for(auto element : p_data){
        p_outputStream << element << std::endl ;
    }
    return p_outputStream;
}