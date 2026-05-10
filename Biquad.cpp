#include "Biquad.hh"

namespace AudioApp {
    void Biquad::ProcessScalar( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount ) {
            float* out = outputBuffer;
            const float* in = inputBuffer;

            for( int i{}; i < channelCount; ++i ) {
                for ( int j{}; j < frameCount; ++j ) {
                    x0[i] = *in;
                    *out = b0 * x0[i] + b1 * x1[i] + b2 * x2[i]
                        - a1 * y1[i] - a2 * y2[i];
                    
                    /**
                     * Cascade past sample values down through 
                     * biquad.
                     */
                    y2[i] = y1[i];
                    y1[i] = *out;

                    x2[i] = x1[i];
                    x1[i] = x0[i];

                    out++;
                    in++;
                }   

            }
        }
    void Biquad::Process( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount ) {
        
        /**
         * Two channel NEON-SIMD optimization for stereo channels
         */
#ifdef __ARM_NEON
        if( channelCount == 2 ) {
            
            const float* LInPtr = inputBuffer;
            const float* RInPtr = inputBuffer + frameCount;
            float* outL = outputBuffer;
            float* outR = outputBuffer + frameCount;

            float32x2_t result;

            float32x2_t vb0 = vdup_n_f32( b0 );
            float32x2_t vb1 = vdup_n_f32( b1 );
            float32x2_t vb2 = vdup_n_f32( b2 );
            float32x2_t va1 = vdup_n_f32( a1 );
            float32x2_t va2 = vdup_n_f32( a2 );


            for( int i{}; i < frameCount; ++i ) {
                vecx0 = { *LInPtr++, *RInPtr++ };
                result = vdup_n_f32( 0.0f );

                /**
                 * MLA chain to produce
                 * result = b0 * x0 + b1 * x1 + b2 * x2
                 *     - a1 * y1 - a2 * y2;
                 */
                result = vmla_f32( result, vb0, vecx0 );
                result = vmla_f32( result, vb1, vecx1 );
                result = vmla_f32( result, vb2, vecx2 );
                result = vmls_f32( result, va1, vecy1 );
                result = vmls_f32( result, va2, vecy2 );
                *outL++ = result[0];
                *outR++ = result[1];
                
                /**
                 * Cascade values down the vectors
                 */
                vecy2 = vecy1; 
                vecy1 = result;
                vecx2 = vecx1;
                vecx1 = vecx0;

            }
        }
        else ProcessScalar( inputBuffer, outputBuffer, frameCount, channelCount );
#endif
        // {
        //     float* out = outputBuffer;
        //     const float* in = inputBuffer;

        //     for( int i{}; i < channelCount; ++i ) {
        //         for ( int j{}; j < frameCount; ++j ) {
        //             x0[i] = *in;
        //             *out = b0 * x0[i] + b1 * x1[i] + b2 * x2[i]
        //                 - a1 * y1[i] - a2 * y2[i];
                    
        //             /**
        //              * Cascade past sample values down through 
        //              * biquad.
        //              */
        //             y2[i] = y1[i];
        //             y1[i] = *out;

        //             x2[i] = x1[i];
        //             x1[i] = x0[i];

        //             out++;
        //             in++;
        //         }   

        //     }
        // } 
    }
    void Biquad::ComputeCoefficients() {
        w0 = 2 * M_PI * centerFreqHz_ / Node::settings->SampleRate;
        alpha = sin( w0 ) / ( 2 * q_ );
        A = pow( 10, dBGain_ / 40 );
        switch ( type_ )
        {
            /**
             * Parameter formulas obtained from 
             * Bristow-Johnson's 'Audio EQ Cookbook'
             */
            case FilterType::LOW_PASS:
                a0 = 1 + alpha;
                a1 = -2 * cos( w0 ) / a0;
                a2 = ( 1 - alpha ) / a0;
                b0 = ( 1 - cos( w0 )) / ( 2 * a0 );
                b1 = (1 - cos( w0 )) / a0;
                b2 = b0;
                break;
            case FilterType::HIGH_PASS:
                a0 = 1 + alpha;
                a1 = -2 * cos( w0 ) / a0;
                a2 = ( 1 - alpha ) / a0;
                b0 = (1 + cos( w0 )) / ( 2 * a0 );
                b1 = -1 * ( 1 + cos( w0 ) ) / a0;
                b2 = b0;
                break;
            case FilterType::LOW_SHELF:
                a0 = ( A + 1 ) + ( A - 1 ) * cos( w0 ) 
                    + 2 * sqrt( A ) * alpha;
                a1 = -2 * ( ( A - 1 ) + ( A + 1 ) * cos( w0 ) )
                    / a0;
                a2 = ( A + 1 + ( A - 1 ) * cos( w0 ) - 2 * sqrt( A ) * alpha )
                    / a0;
                b0 = A * ( A + 1 - ( A - 1 ) * cos( w0 ) + 2 * sqrt( A ) * alpha )
                    / a0;
                b1 = 2 * A * ( ( A - 1 ) - ( A + 1 ) * cos( w0 ) )
                    / a0;
                b2 = A * ( A + 1 - ( A - 1 ) * cos( w0 ) - 2 * sqrt( A ) * alpha )
                    / a0;
                break;
            case FilterType::HIGH_SHELF:
                a0 = ( A + 1 ) - ( A - 1 ) * cos( w0 ) 
                    + 2 * sqrt( A ) * alpha;
                a1 = 2 * ( ( A - 1 ) - ( A + 1 ) * cos( w0 ) )
                    / a0;
                a2 = ( A + 1 - ( A - 1 ) * cos( w0 ) - 2 * sqrt( A ) * alpha )
                    / a0;
                b0 = A * ( A + 1 + ( A - 1 ) * cos( w0 ) + 2 * sqrt( A ) * alpha )
                    / a0;
                b1 = -2 * A * ( ( A - 1 ) + ( A + 1 ) * cos( w0 ) )
                    / a0;
                b2 = A * ( A + 1 + ( A - 1 ) * cos( w0 ) - 2 * sqrt( A ) * alpha )
                    / a0;
                break;
            case FilterType::BELL:
                a0 = 1 + alpha / A;
                a1 = -2 * cos( w0 ) / a0;
                a2 = ( 1 - alpha / A ) / a0;
                b0 = ( 1 + alpha * A ) / a0;
                b1 = ( -2 * cos( w0 ) ) / a0;
                b2 = ( 1 - alpha * A ) / a0;
                break;

        }
    }

}