#pragma once
#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include "Node.hh"

namespace AudioApp {

    class FIRFilter : public Node {
    public:
        FIRFilter( std::string name,
                   std::vector<float> coefficients,
                   const SampleSettings* settings );

        void Process( const float* inputBuffer, float* outputBuffer,
                      std::size_t frameCount, std::size_t channelCount ) override;

        void Reset();

        // (N-1)/2 for symmetric h. We'll trust the caller to pass a symmetric h
        // for now and validate in a later session.
        std::size_t GroupDelaySamples() const;

        std::size_t NumTaps() const { return h_.size(); }

    private:

        std::vector<float> h_;     
        std::vector<float> h_rev_;                // coefficients (impulse response)
        std::vector<std::vector<float>> delayLine_;  // [channel][2*N], double buffer
        std::vector<std::size_t> writeIdx_;          // [channel], current write position
    };

} // namespace AudioApp