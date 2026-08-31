#include <cstddef>
#include <cmath>
#include <algorithm>
#include "Node.hh"
#include <atomic>
#include <iostream>
#ifdef __ARM_NEON
#include <arm_neon.h>
#endif
#pragma once

namespace AudioApp
{
    struct Biquad_Coeffs{
        float a0, a1, a2, b0, b1, b2;
    };
    enum class FilterType {
            LOW_PASS,
            HIGH_PASS,
            HIGH_SHELF,
            LOW_SHELF,
            BELL,
        };

    class Biquad : public Node {
    public:
        void PrintCoefficients() {
            std::cout << "a0 = " << Coeffs->a0 << "\n"
                << "a1 = " << Coeffs->a1 << "\n"
                << "a2 = " << Coeffs->a2 << "\n"
                << "b0 = " << Coeffs->b0 << "\n"
                << "b1 = " << Coeffs->b1 << "\n"
                << "b2 = " << Coeffs->b2 << "\n";
            std::cout << "alpha = " << alpha << '\n'
                << "w0 = " << w0 << '\n'
                << "A = " << A << '\n';
        }
        void Process( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount) override;
        void ProcessScalar( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount);
    
        Biquad( std::string name, FilterType type, double centerFreqHz, double dBGain, double q )
            : type_( type ), q_( q ), dBGain_( dBGain ), centerFreqHz_( centerFreqHz ) {
                this->name = name;
            }
        void SetQ( double q ) { 
            q_ = std::clamp<double>( q, 0.1, 10 ); 
            ComputeCoefficients(); 
        }
        void SetCenterFreqHz( double centerFreqHz ) {  
            centerFreqHz_ = std::clamp<double>( centerFreqHz, 20, 20000 ); 
            ComputeCoefficients(); 
        } 
        void SetdBGain( double dBGain ) { 
            dBGain_ = std::clamp<double>( dBGain, -18, 18 ); 
            ComputeCoefficients(); 
        }
        void SetFilterType( FilterType type ) {
            type_ = type;
            ComputeCoefficients();
        }

        void ChangeSampleSettings() {
            ComputeCoefficients();
            ResizeNumChannels( Node::settings->NumChannels );
#ifdef __ARM_NEON
            vecx0 = vecx1 = vecx2 = vecy1 = vecy2 = vdup_n_f32( 0.0f );
#endif
        };
        void Reset() { 
            for ( auto* v : {&x0, &x1, &x2, &y1, &y2} )
                v->assign( v->size(), 0.0f );
        }
         
    protected:
        void Prepare() override { ChangeSampleSettings(); }

    private:  
        void ComputeCoefficients();
        void ResizeNumChannels( size_t NumChannels ) {
            x0.resize( NumChannels );
            x1.resize( NumChannels );
            x2.resize( NumChannels );
            y1.resize( NumChannels );
            y2.resize( NumChannels );
        }
        
        FilterType type_;
        
        //Readable parameter values 
        double q_;
        double dBGain_; 
        double centerFreqHz_; 
        
        //Used to compute coefficients; based on parameter values
        double alpha;
        double w0;
        double A; 

        //Scalar delay lines per channel
        std::vector<float> x0, x1, x2, y1, y2;
#ifdef __ARM_NEON
        float32x2_t vecx0, vecx1, vecx2, vecy1, vecy2;
#endif
        //Coefficients
        Biquad_Coeffs Coeffs[2];
        std::atomic<int> a_Coeffs_idx;
    };
} // namespace AudioApp
