#include "Node.hh"

namespace AudioApp
{   
    class AudioEngine; //forward declaration

    /**
     * OutputNode controlled by AudioEngine, cannot have any outputs
     * and does not effect audio at all within its Process() method.
     */
    class OutputNode : public Node {
        friend class AudioEngine;
    public:
        
    private:

        OutputNode ( const SampleSettings* settings_ ) : Node( settings_ ) {
            Node::name = "Output";
        };

        void Process( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount) override {
            
        };
        void ProcessNode() override {
            
            /**
             * Sum all inputs together into this output node,
             * directly put it into outputBuffer_.
             */
            currentIndegree = indegree;
            std::fill( outputBuffer_.begin(), outputBuffer_.end(), 0.0f );
            for( auto& input : inputs_ ) {
                for( int i{}; i < outputBuffer_.size(); ++i ) {

                    outputBuffer_[i] += input->outputBuffer_[i];
                }
            }
        }
        
    };
} // namespace AudioApp
