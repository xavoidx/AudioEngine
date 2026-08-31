#include "portaudio.h"
#include "envelope.hh"
#include "Node.hh"
#include <cassert>
#pragma once 

namespace AudioApp {

    enum WaveShape {
        sine, 
        saw,
        square, 
        triangle,
    };
    
    class Oscillator : public Node {
    public:

        WaveShape waveShape; 
        Envelope envelope;

        Oscillator();
        Oscillator( std::string name, int freq, double amplitude, 
            WaveShape waveShape );
        
        void Process( const float* inputBuffer, float* outputBuffer, 
            std::size_t numFrames, std::size_t numChannels ) override; 
        void SetSampleSettings( const SampleSettings* settings );
        
        /**
         * Envelope ADSR values get/set
         */
        void SetAttack( float Attack_mS ) {
            envelope.setAttack( Attack_mS );
        }
        void SetDecay( float Decay_mS ) {
            envelope.setDecay( Decay_mS );
        }
        void SetSustain( float Sustain_Amp ) {
            envelope.setSustain( Sustain_Amp );
        }
        void SetRelease( float Release_mS ) {
            envelope.setRelease( Release_mS );
        }
  
        const SampleSettings* GetSampleSettings() const; 
        
        void SetFreq( int freq ) { freq_ = std::clamp<int> ( freq, 20, 20000 ); }
        int GetFreq() { return freq_; }
        void SetAmplitude( double AmplitudedB );
        double GetAmplitude();
 
    protected:
        void Prepare() override { envelope.settings = settings; }

    private:
        int freq_; 
        double amplitude_; 
        double state;
    };
}

