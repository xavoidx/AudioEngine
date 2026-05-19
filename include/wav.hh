#include <vector>
#include <bit>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>

#pragma once
struct header {
    char RIFF[4];
    uint32_t size;
    char WAVE[4];
};

struct format {
    char FMT[4];
    uint32_t pcm;
    uint16_t format;
    uint16_t channels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

struct data {
    char DATA[4];
    uint32_t dataSize;
};

class WavFile
{
public:
    
    WavFile();
    
    bool read(const char* filename);
    bool write(const char* filename);
    void foo();
    
    std::vector<double>& getSamples() {return samples;}
    uint32_t getSampleRate() {return sampleRate;}
    uint16_t getChannels() {return channels;}
    uint16_t getBitsPerSample() {return bitsPerSample;}
    uint32_t getSize() {return size;}
    
private:
    
    uint32_t updateSize();
    std::vector<double> samples;
    uint32_t sampleRate;
    uint16_t channels;
    uint16_t bitsPerSample;
    uint32_t size;
};




