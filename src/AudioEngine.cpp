#include "AudioEngine.hh"

namespace AudioApp {

    void AudioEngine::PrintNodeList() {

        for( auto& node : Nodes ) {
            node->PrintNode();
        }
        output_->PrintNode();
    }
    void AudioEngine::Nodal_Process( float* inputBuffer, float* outputBuffer ) {

        const std::vector<Node*>& order =
            processOrder_[ activeOrder_.load( std::memory_order_acquire ) ];

        for( Node* node : order ) {
            node->ProcessNode();
        }

        auto outPtr = output_.get()->outputBuffer_.data();
        Interleave( outPtr, outputBuffer, settings_.BufferSize, settings_.NumChannels );
    }

    void AudioEngine::BuildProcessOrder() {

        /*Double Buffer for thread safety*/
        const int next = activeOrder_.load( std::memory_order_relaxed ) ^ 1;
        std::vector<Node*>& order = processOrder_[next];
        order.clear();
        order.reserve( Nodes.size() + 1 );

        std::queue<Node*> processQueue;
        for( auto& node : Nodes ) {
            node->currentIndegree = node->indegree;
            if( node->indegree == 0 ) {
                processQueue.push( node.get() );
            }
        }
        output_->currentIndegree = output_->indegree;
        if( output_->indegree == 0 ) {
            processQueue.push( output_.get() );
        }

        while( processQueue.size() != 0 ) {
            Node* curr = processQueue.front();
            processQueue.pop();
            order.push_back( curr );

            for( auto& outputNode : curr->outputs_ ) {
                outputNode->currentIndegree--;
                if( outputNode->currentIndegree == 0 ) {
                    processQueue.push( outputNode );
                }
            }
        }

        activeOrder_.store( next, std::memory_order_release );
    }

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

    void AudioEngine::Connect( Node* from, Node* to ) {
        
        if(CheckIfCyclic( to, from )) return;
        from->outputs_.push_back( to );
        to->inputs_.push_back( from );
        to->indegree++;
        BuildProcessOrder();
    }

    void AudioEngine::ConnectToOutput( Node* from ) {
        from->outputs_.push_back( output_.get() );
        output_->inputs_.push_back( from );
        output_->indegree++;
        BuildProcessOrder();
    }

} //End Namespace AudioApp
