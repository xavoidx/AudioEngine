//
//  Created by Owen Roush on 6/2/25.
//
#include "AudioApp.hh"
#include "FIRFilter.hh"
#include <chrono>
#define GL_SILENCE_DEPRECATION

// Session Parameters
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 2
#define BUFFER_SIZE 256
static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int Callback(const void *input,
             void *output,
             unsigned long frameCount,
             const PaStreamCallbackTimeInfo *timeInfo,
             PaStreamCallbackFlags statusFlags,
             void *userData);

int main() {
    PaError err = Pa_Initialize();

    if (err != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    PaStream *Stream;
    AudioApp::SampleSettings settings{BUFFER_SIZE, SAMPLE_RATE, NUM_CHANNELS};
    /* auto Osc1 = AudioApp::Oscillator( 440, 1.0, AudioApp::triangle, &settings );
    Osc1.SetAttack( 500.0f );
    Osc1.SetDecay( 100.0f );
    Osc1.SetSustain( 1.0f );
    Osc1.SetRelease( 200.0f ); */

    auto engine = AudioApp::AudioEngine(settings);
    auto osc1 = engine.AddNode<AudioApp::Oscillator>("Oscillator1", 689, 1.0, AudioApp::sine);

    auto filter1 = engine.AddNode<AudioApp::Biquad>
        ( "Biquad1", AudioApp::FilterType::LOW_PASS, 3000, 18, 0.707 );
    auto filter2 = engine.AddNode<AudioApp::Biquad>
        ( "Biquad1", AudioApp::FilterType::BELL, 5000, -18, 3 );
    osc1->SetAttack(500.0f);
    osc1->SetDecay(100.0f);
    osc1->SetSustain(1.0f);
    osc1->SetRelease(200.0f);
//engine.ConnectToOutput(osc1); 
    /* std::vector<float> in(BUFFER_SIZE * 2, 0.5f);
    std::vector<float> out(BUFFER_SIZE * 2, 0.0f);
    const int iterations = 1000000;

    auto t0 = std::chrono::high_resolution_clock::now();
    for( int i = 0; i < iterations; i++ )
    filter1->Process( in.data(), out.data(), BUFFER_SIZE, 2 );
    auto t1 = std::chrono::high_resolution_clock::now();
    for( int i = 0; i < iterations; i++ )
    filter1->ProcessScalar( in.data(), out.data(), BUFFER_SIZE, 2 );
    auto t2 = std::chrono::high_resolution_clock::now();


    auto neon_us  = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    auto scalar_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    printf( "NEON:   %ld us\nScalar: %ld us\n", neon_us, scalar_us ); */

    Pa_OpenDefaultStream(&Stream, NUM_CHANNELS, NUM_CHANNELS,
                         paFloat32, SAMPLE_RATE, BUFFER_SIZE, Callback, &engine);
    Pa_StartStream(Stream);

    /**
     * Perform Processing
     */

    Pa_StopStream(Stream);
    Pa_CloseStream(Stream);
    Pa_Terminate();

    return 0;
}

int Callback(const void *inputBuffer,
             void *outputBuffer,
             unsigned long frameCount,
             const PaStreamCallbackTimeInfo *timeInfo,
             PaStreamCallbackFlags statusFlags,
             void *userData)
{

    auto engine = (AudioApp::AudioEngine *)userData;
    float *out = (float *)outputBuffer;
    float *in = (float *)inputBuffer;

    engine->Nodal_Process(in, out);

    return paContinue;
}
