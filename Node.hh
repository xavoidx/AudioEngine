#pragma once
#include <vector>
#include <string>
#include "AudioTypes.hh"
#include <iostream>

namespace AudioApp {

    class AudioEngine; 
    class OutputNode;

    class Node {
    friend class AudioEngine;
    friend class OutputNode;
    
    public:

        std::string name; 
        virtual ~Node() = default;
        Node( const SampleSettings* settings ) : settings(settings) {

            inputBuffer_.resize( settings->BufferSize * settings->NumChannels, 0.0f );
            outputBuffer_.resize( settings->BufferSize * settings->NumChannels, 0.0f );
            auto x = settings->BufferSize;

        };

        virtual void Process( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount) = 0;
        
        void PrintInputs() {
            std::cout << "The inputs of " << name << " are:\n";
            for( auto& input : inputs_ ) {
                std::cout << input->name << ", ";
            }
            std::cout << "\n";
        }
        void PrintOutouts() {
            std::cout << "The outputs of " << name << " are:\n";
            for( auto& output : outputs_ ) {
                std::cout << output->name << ", ";
            }
            std::cout << "\n";
        }
        void PrintNode() {
            std::cout << "Node: " << name << ", Indegree " << indegree <<
                ", CurrentIndegree " << currentIndegree << '\n';
        }
        void PrintSettings() {
            std::cout << settings->BufferSize << ", " <<
                settings->NumChannels << ", " << settings->SampleRate
                << "\n";
        }
    protected:
        const SampleSettings* settings;
        
    private:

        virtual void ProcessNode();
        std::vector<Node*> inputs_;
        std::vector<Node*> outputs_;

        std::vector<float> inputBuffer_;
        std::vector<float> outputBuffer_;

        

        
        short indegree{};
        short currentIndegree{};

    };

} //End namespace AudioApp
