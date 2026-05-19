#include "Node.hh" 
#include <iostream>
namespace AudioApp {

    void Node::ProcessNode() {
        currentIndegree = indegree;
        std::fill( inputBuffer_.begin(), inputBuffer_.end(), 0.0f );
        for( auto& input : inputs_ ) {
            for( int i{}; i < inputBuffer_.size(); ++i ) {
                inputBuffer_[i] += input->outputBuffer_[i];
            }
        }
        

        /* auto x = settings->BufferSize;
        auto y = settings->NumChannels;
        auto x1 = inputBuffer_.data();
        auto y1 = outputBuffer_.data(); */
        
        Process( inputBuffer_.data(), outputBuffer_.data(),
            settings->BufferSize, settings->NumChannels );
        

        
    }
}