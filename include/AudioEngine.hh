#include "AudioTypes.hh"
#include "OutputNode.hh"
#include "AudioUtils.hh"
#include "Node.hh"
#include "osc.hh"
#include <iostream>
#include <stack>
#include <unordered_set>
#include <cassert>
#include <algorithm>
#include <memory>
#include <vector>
#include <queue>
#include <atomic>
#pragma once

namespace AudioApp {
    class AudioEngine {
    public:

        /**
         * Kahn's alogrithm topological sort of nodes
         */
        std::vector<std::unique_ptr<Node>> Nodes;
        void Nodal_Process( float* inputBuffer, float* outputBuffer );
        void BuildProcessOrder();
        void PrintNodeList();

        AudioEngine( SampleSettings settings ) : settings_( settings ), 
            output_( std::unique_ptr<OutputNode>( new OutputNode() ) ) {
                PrepareNode( output_.get() );
            };
        
        /**
         * Factory method to create nodes and make sure that
         * the AudioEngine owns them.
         */
        template<typename T, typename... Args>
        T* AddNode( Args&&... args ) {
            auto node = std::make_unique<T>( std::forward<Args>(args)... );

            PrepareNode( node.get() );

            T* ptr = node.get();
            Nodes.push_back( std::move(node) );
            BuildProcessOrder();
            return ptr;
        }

        void Connect( Node* from, Node* to );
        void ConnectToOutput( Node* from );
        SampleSettings settings_; 
    private: 

        void PrepareNode( Node* node ) {
            node->settings = &settings_;
            node->inputBuffer_.resize( settings_.BufferSize * settings_.NumChannels, 0.0f );
            node->outputBuffer_.resize( settings_.BufferSize * settings_.NumChannels, 0.0f );
            node->Prepare();
        }

        void RemoveNode( Node* );
        std::unique_ptr<OutputNode> output_;
        bool CheckIfCyclic( Node* from, Node* to );
        
        std::vector<Node*> processOrder_[2];
        std::atomic<int> activeOrder_{};
    };
} 
