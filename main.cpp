//
//  Created by Owen Roush on 6/2/25.
//
#include "ImPlot/implot-master/implot.h"
#include "ImPlot/imgui-master/backends/imgui_impl_glfw.h"
#include "imgui.h"
#include "imgui_impl_opengl2.h"
#include <GLFW/glfw3.h>
#include "AudioApp.hh"
#include "FIRFilter.hh"
#include <chrono>
#define GL_SILENCE_DEPRECATION

// Session Parameters
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 2
#define BUFFER_SIZE 256
static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

int Callback( const void *input, 
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

int main() {
    
    PaError err = Pa_Initialize();

    if(err != paNoError ) {
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
        return 1;
    }

    PaStream* Stream; 
    AudioApp::SampleSettings settings { BUFFER_SIZE, SAMPLE_RATE, NUM_CHANNELS };
    /* auto Osc1 = AudioApp::Oscillator( 440, 1.0, AudioApp::triangle, &settings );
    Osc1.SetAttack( 500.0f );
    Osc1.SetDecay( 100.0f );
    Osc1.SetSustain( 1.0f );
    Osc1.SetRelease( 200.0f ); */

    auto engine = AudioApp::AudioEngine( settings );
    auto osc1 = engine.AddNode<AudioApp::Oscillator>( "Oscillator1", 440, 1.0, AudioApp::sine );

    /* auto filter1 = engine.AddNode<AudioApp::Biquad> 
        ( "Biquad1", AudioApp::FilterType::LOW_PASS, 3000, 18, 0.707 ); */
    /* auto filter2 = engine.AddNode<AudioApp::Biquad> 
        ( "Biquad1", AudioApp::FilterType::BELL, 5000, -18, 3 ); */
    osc1->SetAttack( 500.0f );
    osc1->SetDecay( 100.0f );
    osc1->SetSustain( 1.0f );
    osc1->SetRelease( 200.0f );
    engine.ConnectToOutput( osc1 );
    AudioApp::SampleSettings testSettings { 5, SAMPLE_RATE, 1 };
    auto filter = AudioApp::FIRFilter( "TestFilter", 
        std::vector<float>{1, 2, 3, 4, 5}, &testSettings );
    
    float* testIn = new float[10];
    std::fill( testIn, testIn + 10, 0.0f );
    testIn[0] = 1.0f;
    float* testOut = new float[10];
    std::fill( testOut, testOut + 10, 0.0f );
    
    filter.Process( testIn, testOut, 5, 1 );
    for( int i{}; i < 10; ++i ) {
        std::cout << testOut[i] << ' ';
    }
    std::cout << '\n';

    delete testIn;
    delete testOut;


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

    /**
     * Initialize PortAudio Stream
     */
    
    Pa_OpenDefaultStream( &Stream, NUM_CHANNELS, NUM_CHANNELS, 
        paFloat32, SAMPLE_RATE, BUFFER_SIZE, Callback, &engine );
    Pa_StartStream(Stream); 

    /**
     * Initialize UI framework
     */
    glfwSetErrorCallback(glfw_error_callback);
    if( !glfwInit() ) {
        return 1;
    }

    float main_scale = 
        ImGui_ImplGlfw_GetContentScaleForMonitor( glfwGetPrimaryMonitor() );
    GLFWwindow* window = glfwCreateWindow( 
        (int)( 1280 * main_scale ), 
        (int)( 800 * main_scale ),
        "Dear ImGui GLFW+OpenGL2 example",
        nullptr,
        nullptr 
    );
    if ( window == nullptr )
        return 1;
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 1 );

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; 
    
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);     
    style.FontScaleDpi = main_scale; 
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    bool show_demo_window = false;
    bool show_another_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static int freq = 440.0f;
            static float amplitude = 0.0f;
            osc1->SetFreq( freq );
            osc1->SetAmplitude( amplitude );

            ImGui::Begin("Oscillator1");                          // Create a window called "Hello, world!" and append into it.

            ImGui::SliderInt("Frequency", &freq, 20, 20000, "%d", ImGuiSliderFlags_Logarithmic );     
            ImGui::SliderFloat("Amplitude", &amplitude, -200.0f, 0.0f, "%0.1f", ImGuiSliderFlags_Logarithmic );          // Edit 1 float using a slider from 0.0f to 1.0f
            
            if( ImGui::Button( "Press" ) ) {
                osc1->envelope.Actuate();
            }
            
            if ( ImGui::Button( "Release" ) ) {
                osc1->envelope.Release();
            }

            ImGui::Text("Amplitude = %0.1f dB", amplitude);
            ImGui::Text("Frequency = %d Hz", osc1->GetFreq() );


            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        //glUseProgram(last_program);

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    Pa_StopStream(Stream);
    Pa_CloseStream(Stream);
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    
    Pa_Terminate(); 

    return 0;
}

int Callback( const void *inputBuffer, 
                void *outputBuffer,
                unsigned long frameCount,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void *userData ) {

    auto engine = (AudioApp::AudioEngine*) userData;
    float *out = (float*)outputBuffer;
    float *in = (float*)inputBuffer;

    /**
     * TODO: input AudioEngine that has a interleave and
     * reverse interleave method. 
     */
    engine->Nodal_Process( in, out );
 
    return paContinue;

}
