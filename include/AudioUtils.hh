#include <cstddef>
#pragma once

namespace AudioApp {
    /**
         * PortAudio expects buffers to interleaved based on how many
         * channels are being used. AudioApp prefers to do its DSP
         * on uninterleaved or planar samples
         * 
         * e.g. 
         * Buffer = { L0, R0, L1, R1, L2, R2... LN, RN } // INTERLEAVED
         * Buffer = { L0, L1, L2.... LN, R0, R1, R2... RN } //NON-INTERLEAVED (planar)
         * 
         * The Interleave() and ReverseInterleave() methods are used so that
         * all processing in back end can be done on non-interleaved samples,
         * then returned to the callback as interleaved samples.
         */
    void Interleave( float* inputBuffer, float* outputBuffer, 
        std::size_t frameCount, std::size_t channelCount );
    
    void ReverseInterleave( float* inputBuffer, float* outputBuffer, 
        std::size_t frameCount, std::size_t channelCount );
}