#pragma once
namespace AudioApp {
    struct SampleSettings {

        int BufferSize; 
        int SampleRate; 
        int NumChannels;

    };

    struct AudioBuffer {

        float** inputBuffer;
        int frameCount;
        int numChannels;

    };
}