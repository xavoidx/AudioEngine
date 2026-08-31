#include "FIRFilter.hh"

namespace AudioApp {
    FIRFilter::FIRFilter( std::string name, std::vector<float> coefficients )
        : h_( coefficients ) {
            h_rev_ = h_;
            std::reverse( h_rev_.begin(), h_rev_.end() );
            this->name = name;
        };

    void FIRFilter::Prepare() {
        delayLine_.resize( settings->NumChannels );
        for( auto& vec : delayLine_ ) {
            vec.assign( h_.size() * 2, 0.0f );
        }
        writeIdx_.assign( settings->NumChannels, 0 );
    }

    void FIRFilter::Process( const float* inputBuffer, float* outputBuffer,
                      std::size_t frameCount, std::size_t channelCount ) {

        const float* inPtr = inputBuffer;
        float* outPtr = outputBuffer;
        const std::size_t N = h_.size();

#ifdef __ARM_NEON // Parallelized tap-domain SIMD 
        for ( int i{}; i < channelCount; ++i ) {    

            float* ptr_currDelayLine = delayLine_[i].data();
            const float* ptr_h_rev_ = h_rev_.data();
            std::size_t& currIdx = writeIdx_[i];

            for( std::size_t j{}; j < frameCount; ++j ) {
                
                /**
                 * Double buffer design eliminates need for pointer wrap-around;
                 * this speeds up architecture by eliminating any slow modulus
                 * operators, as well as gives NEON SIMD a full address space to load
                 * 4 bytes at a time uninterrupted by pointer wraps. 
                 * [ n-1, n, n-3, n-2, n-1, n, n-3, n-2 ]
                 *        ^   ^             ^
                 *      curr read -> -> read stop
                 */
                const float NewSample = *inPtr++;
                *( ptr_currDelayLine + currIdx ) = NewSample;
                *( ptr_currDelayLine + currIdx + N ) = NewSample;
                
                /**
                 * Vectorized dot product in groups of 4 to 
                 * fill the 4 float capacity of float32x_t NEON intrinsic. 
                 * For each pass, the acc_vector will hold { acc[k], acc[k+1], acc[k+2], acc[k+3] }
                 **/
                std::size_t K_vec = N / 4 * 4;
                float32x4_t acc_vector = vdupq_n_f32( 0.0f );
                for( std::size_t k{}; k < K_vec; k += 4 ) {
                    float32x4_t v_h_rev_ = vld1q_f32( ptr_h_rev_ + k );
                    float32x4_t v_samples = vld1q_f32( ptr_currDelayLine + currIdx + k + 1 );
                    acc_vector = vmlaq_f32( acc_vector, v_h_rev_, v_samples );
                }

                /**
                 * Scalar dot product for remaining taps for N % 4 != 0.
                 * Begin k where the vector product left off; within the last
                 * 4 taps of N
                 **/
                float acc_scalar{};
                for( std::size_t k = K_vec; k < N ; ++k ) {
                    acc_scalar += ptr_h_rev_[k] * ptr_currDelayLine[ currIdx + k + 1 ];
                }

                
                currIdx++;
                if( currIdx >= N ) currIdx = 0;
                *outPtr++ = vaddvq_f32( acc_vector ) + acc_scalar; 
            }
        }


#else        
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
#endif //end __ARM_NEON if
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