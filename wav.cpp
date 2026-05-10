//
//  wav.cpp
//  Hello World
//
//  Created by Owen Roush on 11/27/25.
//
#include "wav.hh"
WavFile::WavFile()
{
    sampleRate = 0;
    bitsPerSample = 0;
    channels = 0;
    size = 0;
}

bool WavFile::read(const char* filename)
{
    std::ifstream inputFile;
    inputFile.open(filename, std::ios::binary);
    if(!inputFile)
    {
        std::cout << "File could not be opened\n";
        return false;
    }
    
    header header;
    format format;
    data data;
    
    
    inputFile.read((char*) &header, sizeof(header));
    if(memcmp(header.RIFF, "RIFF", 4))
    {
        std::cout << "Incorrect Wav Format\n";
        return false;
    }
    
    char chunkID[4];
    while(inputFile.read(chunkID, 4))
    {
        if(!memcmp(chunkID, "fmt ", 4))
        {
            inputFile.read((char*)(&format.pcm), sizeof(format)-4);
            break;
        }
        else
        {
            uint16_t junkSize;
            inputFile.read((char*)&junkSize, 4);
            inputFile.seekg(junkSize, std::ios::cur);
        }
    }

    inputFile.read((char*)&data, sizeof(data));
    int totalSamples = data.dataSize / (format.channels * 2);
    std::vector<int16_t> samples16(totalSamples);
    inputFile.read((char*)samples16.data(), totalSamples*sizeof(int16_t));
    samples.clear();
    samples.resize(totalSamples);
    for(int i = 0; i<totalSamples; i++)
    {
        this->samples[i] = samples16[i] / 32768.0;
    }

    this->sampleRate = format.sampleRate;
    this->bitsPerSample = format.bitsPerSample;
    this->size = header.size;
    this->channels = format.channels;
    inputFile.close();
    return true;
}
bool WavFile::write(const char* filename)
{
    std::ofstream outputFile(filename, std::ios::binary);
    if(outputFile.is_open())
    {
        
        uint32_t bytesRate = (this->sampleRate)*(this->bitsPerSample)*(this->channels)/8;
        uint16_t blockAlign = this->channels * this->bitsPerSample / 8;
        
        header outHeader = {{'R','I','F','F'}, this->size, {'W','A','V','E'}};
        format outFormat = {{'f','m','t',' '}, 16, 1, this->channels, this->sampleRate, bytesRate, blockAlign, this->bitsPerSample};
        uint32_t dataSize = (uint32_t) this->samples.size() * this->bitsPerSample / 8;
        data outData = {{'d','a','t','a'}, dataSize};
        
        outputFile.write(reinterpret_cast<char*>(&outHeader), sizeof(header));
        outputFile.write(reinterpret_cast<char*>(&outFormat), sizeof(format));
        outputFile.write(reinterpret_cast<char*>(&outData), sizeof(data));
        
        std::vector<int16_t> outSamples(this->samples.size());
        std::cout << this->samples[0] << "\n";
        for(int i = 0; i<this->samples.size(); i++)
        {
            outSamples[i] = this->samples[i] * 32768;
        }
        outputFile.write(reinterpret_cast<char*>(outSamples.data()), outSamples.size()*sizeof(int16_t));
        
        return true;
    }
    else
    {
        std::cout << "File could not be written";
        return false;
    }
}





