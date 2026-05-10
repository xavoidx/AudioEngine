#include "AudioUtils.hh"

namespace AudioApp {

    void Interleave( float* inputBuffer, float* outputBuffer, 
        std::size_t frameCount, std::size_t channelCount ) {

        float* inPtr = inputBuffer;
        float* outPtr = outputBuffer;
        for( int i{}; i < frameCount; i++ ) {
            for( int j{}; j < channelCount; j++ ) {
                *outPtr++ = *(inPtr + j * frameCount);
            }
            inPtr++;
        }

    }
    void ReverseInterleave( float* inputBuffer, float* outputBuffer, 
        std::size_t frameCount, std::size_t channelCount ) {
        
        float* inPtr = inputBuffer;
        float* outPtr = outputBuffer;
        for( int i{}; i < channelCount; i++ ) {
            for( int j{}; j < frameCount; j++ ) {
                *outPtr++ = *(inPtr + j * channelCount);
            }
            inPtr++;
        }
    }
}   