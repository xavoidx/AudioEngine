#include "envelope.hh"
#include <iostream>
namespace AudioApp {

    float Envelope::GetNextSample() {
        
        //Attack stage
        if( target == 1.0f ) { 
            multiplier = ( 1 - attack_alpha_ ) * target
                + attack_alpha_ * prev_Y;
            prev_Y = multiplier;
        }
        //Flip to decay stage
        if( multiplier > 0.999f ) {
            target = sustain_;
        }

        //Decay Stage
        if( target == sustain_ ) {
            multiplier = ( 1 - decay_alpha_ ) * target
                    + decay_alpha_ * prev_Y;
            prev_Y = multiplier;
        }

        //Release Stage
        if( target == 0.0f ) {
            multiplier = ( 1 - release_alpha_ ) * target
                + release_alpha_ * prev_Y;
            prev_Y = multiplier;
        }
        return multiplier;
    }
}