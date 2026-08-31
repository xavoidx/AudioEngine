#include "Node.hh" 
#include <iostream>
namespace AudioApp {

    void Node::ProcessNode() {
        std::fill( inputBuffer_.begin(), inputBuffer_.end(), 0.0f );
        for( auto& input : inputs_ ) {
            for( int i{}; i < inputBuffer_.size(); ++i ) {
                inputBuffer_[i] += input->outputBuffer_[i];
            }
        }
        
        Process( inputBuffer_.data(), outputBuffer_.data(),
            settings->BufferSize, settings->NumChannels );  
    }
}