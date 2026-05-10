#include "osc.hh"
#include <vector>
#pragma once
 
namespace AudioApp {
    class Synth : Processor {

    public:

        float Process() override;
        void AddOscillator(Oscillator* osc);
        Synth( const SampleSettings* settings );
        Oscillator& getVoice(int index);

    private: 

        std::vector<Oscillator*> voices{}; 
        const SampleSettings* settings;

    };

}; // End namespace AudioApp