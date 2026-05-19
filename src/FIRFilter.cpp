#include "FIRFilter.hh"

namespace AudioApp {
    FIRFilter::FIRFilter( std::string name, std::vector<float> coefficients,
        const SampleSettings* settings) : Node( settings ), h_( coefficients ) {
            delayLine_.resize( settings->NumChannels );
            for( auto& vec : delayLine_ ) {
                vec.assign( coefficients.size() * 2, 0.0f );
            }
            writeIdx_.assign( settings->NumChannels, 0 );
            h_rev_ = h_;
            std::reverse( h_rev_.begin(), h_rev_.end() );

        };
    void FIRFilter::Process( const float* inputBuffer, float* outputBuffer,
                      std::size_t frameCount, std::size_t channelCount ) {

        const float* inPtr = inputBuffer;
        float* outPtr = outputBuffer;
        const std::size_t N = h_.size();

        for ( int i{}; i < channelCount; ++i ) {
            
            std::vector<float>& currDelayLine = delayLine_[i];
            std::size_t& currIdx = writeIdx_[i];
            for( int j{}; j < frameCount; ++j ) {
                float acc{};
                const float NewSample = *inPtr++;
                currDelayLine[ currIdx ] = NewSample;
                currDelayLine[ currIdx + N ] = NewSample;
                
                /**
                 * [ n, n-3, n-2, n-1, n, n-3, n-2, n-1 ]
                 *   ^   ^
                 * curr read
                 */
                for( int k{}; k < N; ++k ) {
                    acc += h_rev_[k] * currDelayLine[ currIdx + 1 + k ];
                    
                }

                currIdx++;
                if( currIdx >= N ) currIdx = 0;

                *outPtr++ = acc;
            }
        }
    }
    void FIRFilter::Reset() {
        for( auto& vec : delayLine_ ) {
            std::fill( vec.begin(), vec.end(), 0.0f );
        }
        std::fill( writeIdx_.begin(), writeIdx_.end(), 0 );
    }
    std::size_t FIRFilter::GroupDelaySamples() const {
        return (h_.size() - 1) / 2;
    }

}