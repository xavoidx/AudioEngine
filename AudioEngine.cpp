#include "AudioEngine.hh"

namespace AudioApp {

    void AudioEngine::PrintNodeList() {

        for( auto& node : Nodes ) {
            node->PrintNode();
        }
        output_->PrintNode();
    }
    void AudioEngine::Nodal_Process( float* inputBuffer, float* outputBuffer ) {
        
        /**
         * Add all nodes in AudioEngine with an indegree of 0
         * to the processQueue;
         */
        std::cerr << "Checkpoint A\n";
        for( auto& node : Nodes ) {
            node->currentIndegree = node->indegree;
            if( node->indegree == 0 ) {
                processQueue_.push( node.get() );
            }
        }
        
        /**
         * Kahn's algorithm topographic traversal of all nodes
         */
        std::cerr << "Checkpoint B\n";
        Node* curr = nullptr;
        while( processQueue_.size() != 0 ) {
            //PrintNodeList();
            curr = processQueue_.front();
            processQueue_.pop();  
            
            curr->ProcessNode();
            std::cerr << "Checkpoint C\n";
            for( auto& outputNode : curr->outputs_ ) {
                outputNode->currentIndegree--; 
                if( outputNode->currentIndegree == 0 ) {
                    processQueue_.push( outputNode );
                }
            }
        } 
        std::cerr << "Checkpoint C\n";
        /**
         * Assume that curr is the last node in the graph, 
         * to the outputBuffer.
         */
        auto outPtr = output_.get()->outputBuffer_.data();
        Interleave( outPtr, outputBuffer, settings_.BufferSize, settings_.NumChannels );
        
    }
    /**
     * TODO: standardize variable names channelCount and frameCount
     */

    bool AudioEngine::CheckIfCyclic( Node* to, Node* from ) {

        //DFS of nodes
        std::stack<Node*> stack; 
        std::unordered_set<Node*> visited;

        stack.push( to );
        while( stack.size() != 0 ) {
           
            Node* curr = stack.top();
            stack.pop();
            
            visited.insert( curr );
            
            if ( curr == from ) return true;
            for( auto& node : curr->outputs_ ) {
                
                if( !visited.contains( node ) ) stack.push( node ); 
                
            }
        }
        return false;
    }

    void AudioEngine::Connect( Node* to, Node* from ) {
        
        assert( !CheckIfCyclic( to, from ) );
        from->outputs_.push_back( to );
        to->inputs_.push_back( from );
        to->indegree++;
        to->currentIndegree++;
        
    }
    void AudioEngine::ConnectToOutput( Node* from ) {
        from->outputs_.push_back( output_.get() );
        output_->inputs_.push_back( from );
        output_->indegree++;
        output_->currentIndegree++;
    }

} //End Namespace AudioApp