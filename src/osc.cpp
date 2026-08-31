/*
 * Implementation for Oscillator class. 
 *
 */
#include "osc.hh"
#include "iostream"
#include <cmath>

namespace AudioApp {

    Oscillator::Oscillator() : waveShape(sine), freq_(440), amplitude_(1.0), state(0.0) {}

    Oscillator::Oscillator( std::string name, int freq, double amplitude, WaveShape waveShape )
        : waveShape(waveShape), freq_(freq), amplitude_(amplitude), state(0.0) {
            this->name = name;
        }

    /**
     * TODO: Implement void Process( inputBuffer* ) such that
     * process function fills entire buffer in memory at once 
     * and doesn't have to be called for each sample. 
     */
    void Oscillator::Process( const float* inputBuffer, float* outputBuffer, 
        std::size_t numFrames, std::size_t numChannels ) {

        float* ch0 = outputBuffer;
        for( int i = 0; i < numFrames; i++ ) {
            switch ( waveShape ) {
            case sine: 
                state += 2 * M_PI * freq_ / Node::settings->SampleRate;
                if( state >= 2 * M_PI )
                    state -= 2 * M_PI;
                *outputBuffer++ = envelope.GetNextSample() * amplitude_ * sin(state); 
                break;
            case saw:
                state += static_cast<double> ( freq_ ) / Node::settings->SampleRate;
                if( state >= 1.0f )
                    state = 0.0f;
                *outputBuffer++ = envelope.GetNextSample() * amplitude_ * (2.0f * state - 1.0f);
                break;
            case square:
                state += static_cast<double> ( freq_ ) / Node::settings->SampleRate; 
                if( state >= 1.0f ) state = 0.0f;
                *outputBuffer++ = envelope.GetNextSample() * amplitude_ * (( state >= 0.5f )? 1.0f : -1.0f);
                break; 
            case triangle:
                state += static_cast<double> ( freq_ ) / Node::settings->SampleRate; 
                if( state >= 1.0f ) state = 0.0f;
                *outputBuffer++ = envelope.GetNextSample() * amplitude_  
                    * (state < 0.5f ? (4.0f * state - 1.0f) : (3.0f - 4.0f * state));
                break;
            };
        }

        for( std::size_t ch = 1; ch < numChannels; ch++ ) {
            std::copy( ch0, ch0 + numFrames, ch0 + ch * numFrames );
        }

    }

    void Oscillator::SetSampleSettings( const SampleSettings* settings_ ) {
        settings = settings_;
    }

    const SampleSettings* Oscillator::GetSampleSettings() const {
        return Node::settings;
    }

    void Oscillator::SetAmplitude( double AmplitudedB ) {
        amplitude_ = std::pow( AmplitudedB / 20, 10 );
    }
    double Oscillator::GetAmplitude() {
        return 20 * log10( amplitude_ );
    }

}
    