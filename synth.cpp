#include "synth.hh"

namespace AudioApp {
    
    //Initilize voices vector with a defulat Oscillator
    Synth::Synth( const SampleSettings* settings_ ) 
        : voices(0), settings(settings_) {} 

    float Synth::Process() {
        float result = 0.0f;
        for( auto& osc : voices )
        {
            result += osc->Process();
        }
        return result;
    }

    Oscillator& Synth::getVoice(int index) 
    {
        return *(voices.at(index));
    }

    void Synth::AddOscillator(Oscillator* osc) 
    {
        voices.push_back(osc);
    }
    


}; // End namespace AudioApp