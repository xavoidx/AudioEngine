#include <cstddef>
#include <cmath>
#include <algorithm>
#include "Node.hh"
#include <iostream>
#ifdef __ARM_NEON
#include <arm_neon.h>
#endif
/**
 * TODO: NEON SIMD vectorizing
 */
namespace AudioApp
{
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
            for( double x : { a0, a1, a2, b0, b1, b2 } ) {
                std::cout << x << ' ';
            }
            std::cout << '\n';
            std::cout << "alpha = " << alpha << '\n';
            std::cout << "w0 = " << w0 << '\n';
            std::cout << "A = " << A << '\n';
        }
        void Process( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount) override;
        void ProcessScalar( const float* inputBuffer, float* outputBuffer,
            std::size_t frameCount, std::size_t channelCount);
    
        Biquad( std::string name, FilterType type, double centerFreqHz, double dBGain, double q, 
            const SampleSettings* settings ) : type_( type ), centerFreqHz_( centerFreqHz ),
            dBGain_( dBGain ), q_( q ), Node( settings ){
                
                this->name = name;
  
                ChangeSampleSettings();
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

        //Sample values per channel
        std::vector<float> x0;
        std::vector<float> x1;
        std::vector<float> x2;
        std::vector<float> y1;
        std::vector<float> y2;


#ifdef __ARM_NEON
        float32x2_t vecx0;
        float32x2_t vecx1;
        float32x2_t vecx2;
        float32x2_t vecy1;
        float32x2_t vecy2;
#endif
        //Coefficients
        float a0;
        float a1;
        float a2;
        float b0; 
        float b1;
        float b2;

    };
} // namespace AudioApp
