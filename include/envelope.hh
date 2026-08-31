#include "AudioTypes.hh"
#include <cmath>
#include <algorithm>
#pragma once

namespace AudioApp {

    class Envelope { 
    friend class Oscillator;
    public:
        
        /**
         * ADSR values are calculated based on sample rate
         * when set; will need some updating function if sample rate
         * changes.
         * 
         * TODO: AudioEngine needs UpdateSampleRate() function that
         * updates all dependant params
         */
       
        bool pressed{};
        const SampleSettings* settings;

        float GetNextSample(); 
        
        /**
         * Trigger methods Actuate() and Release() set target values
         * for recursive RC envelope in GetNextSample() method
         */
        void Actuate() {
            pressed = true;
            multiplier = 0.0f;
            target = 1.0f;
        }
        void Release() {
            pressed = false;
            target = 0.0f;
        }

    private:
        Envelope ( float attack_mS, float decay_mS, float sustain, float release_mS ) 
            : attack_mS_{attack_mS}, decay_mS_{decay_mS}, sustain_{sustain}, 
            release_mS_{release_mS} {

                /**
                 * Conversion of ADR (mS) values to alpha values
                 * for envelope difference equation
                 */
                attack_alpha_ = pow( M_E, -1.0 
                    / ( settings->SampleRate * attack_mS / 1000 ));
                decay_alpha_ = pow( M_E, -1.0 
                    / ( settings->SampleRate * decay_mS / 1000 ));
                release_alpha_ = pow( M_E, -1.0 
                    / ( settings->SampleRate * release_mS / 1000 ));

            }

        Envelope () : attack_mS_{}, decay_mS_{}, sustain_{ 1.0f }, release_mS_{} {}

        float prev_Y{};

        float attack_alpha_{};
        float decay_alpha_{};
        float release_alpha_{};
        
        float attack_mS_ { 0.01f };
        float decay_mS_ { 0.01f };
        float sustain_ { 1.0f };
        float release_mS_ { 0.01f };
        
        float multiplier{ 0.0f };
        float target{ 0.0f };

        void setAttack ( float attack_mS ) {
            attack_mS_ = attack_mS;
            attack_alpha_ = //Recalculating alpha value
                pow( M_E, -1.0 / ( settings->SampleRate * attack_mS / 1000 ));    
        }
        void setDecay ( float decay_mS ) {
            decay_mS_ = decay_mS;
            decay_alpha_ = //Recalculating alpha value
                pow( M_E, -1.0 / ( settings->SampleRate * decay_mS / 1000 ));
        }
        void setRelease ( float release_mS ) {
            release_mS_ = release_mS;
            release_alpha_ = //Recalculating alpha value
                pow( M_E, -1.0 / ( settings->SampleRate * release_mS / 1000 ));
        }
        void setSustain ( float sustain ) {

            sustain_ = std::clamp( sustain, 0.0f, 1.0f );

        }




    };
} 