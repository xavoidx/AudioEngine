/* 
 * Super class for all audio processors. 
 * Acts as the main interface of the float process(); function
 */
#pragma once
#include <cstddef>

namespace AudioApp {

    

    class Processor {
    public: 

        virtual ~Processor() = default;
        virtual void Process( const float* inputBuffer, float* outputBuffer, 
            std::size_t frameCount, std::size_t channelCount) = 0;

    };

}