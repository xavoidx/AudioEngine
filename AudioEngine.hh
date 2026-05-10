#pragma once
#include "AudioTypes.hh"
#include "OutputNode.hh"
#include "AudioUtils.hh"
#include "Node.hh"
#include "processor.hh"
#include "osc.hh"
#include <iostream>
#include <stack>
#include <unordered_set>
#include <cassert>
#include <algorithm>
#include <memory>
#include <vector>
#include <queue>

namespace AudioApp {
    class AudioEngine {
    public:

        std::vector<std::unique_ptr<Node>> Nodes;
        /**
         * Kahn's alogrithm topological sort of nodes
         */
        void Nodal_Process( float* inputBuffer, float* outputBuffer );
        void PrintNodeList();

        AudioEngine( SampleSettings settings ) : settings_( settings ), 
            output_( std::unique_ptr<OutputNode>( new OutputNode ( &settings_ ) ) ) {};
        
        /**
         * Factory method to create nodes and make sure that
         * the AudioEngine owns them.
         */
        template<typename T, typename... Args>
        T* AddNode( Args&&... args ) {
            auto node = std::make_unique<T>( std::forward<Args>(args)..., &settings_ );
            
            static_cast<Node*>(node.get())->settings = &settings_;
            node->inputBuffer_.resize( settings_.BufferSize * settings_.NumChannels, 0.0f );
            node->outputBuffer_.resize( settings_.BufferSize * settings_.NumChannels, 0.0f );
            
            T* ptr = static_cast<T*>( node.get() );
            Nodes.push_back( std::move(node) );
            return ptr;
        }

        void Connect( Node* to, Node* from );
        void ConnectToOutput( Node* from );

        SampleSettings settings_; 
    private: 

        

        void RemoveNode( Node* );
        std::unique_ptr<OutputNode> output_;
        bool CheckIfCyclic( Node* to, Node* from );
        
        /**
         * TODO: ChangeSampleSettings(); 
         */
        
        
        std::queue<Node*> processQueue_; 
        // std::vector<float> scratch1_; 
        // std::vector<float> scratch2_;
    };
} 
