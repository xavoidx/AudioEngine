# AudioEngine — C++ Node-Based Real-Time Audio Engine

A small real-time audio engine for prototyping DSP. Nodes are declared, wired
together into a graph, and processed in dependency order every callback. Built
mainly as a sandbox for trying out filter implementations and SIMD optimizations
against a working audio stream, and as a place to put DSP code I want to hear
rather than just plot.

Runs on macOS/ARM with PortAudio for I/O. NEON paths for the Biquad and FIR.

## Highlights

- Node graph with dependency resolution via Kahn's algorithm topological sort
- Processing order is sorted once on graph mutation, not per callback
- Coefficient and process-order updates published to the audio thread with an
  atomic index flip over a double buffer — no locks or allocations in the callback
- Biquad with the 5 EQ Cookbook filter types, NEON-vectorized two channels at a time
- FIR with a double-buffered delay line and reversed coefficients, so the inner
  loop is a contiguous forward-direction dot product with no modulo
- Planar buffers internally, interleaved only at the PortAudio boundary
- Oscillator with ADSR envelope, four wave shapes

## Writing a Node

Inherit from `Node` and implement `Process()`. Constructors take only your own
parameters; the engine assigns sample settings and calls `Prepare()` before the
node ever runs, so anything that depends on sample rate or channel count goes
there.

```cpp
class Gain : public AudioApp::Node {
public:
    Gain( std::string name, float gain ) : gain_( gain ) { this->name = name; }

    void Process( const float* in, float* out,
        std::size_t frameCount, std::size_t channelCount ) override {

        for( std::size_t i = 0; i < frameCount * channelCount; ++i ) {
            out[i] = in[i] * gain_;
        }
    }

private:
    float gain_;
};
```

Buffers are planar, so channel `c` starts at `frameCount * c`. A node's inputs
are summed into its input buffer before `Process()` is called.

## Building a Graph

`AddNode` constructs the node, hands it the engine's sample settings, and takes
ownership. `Connect( from, to )` adds an edge and rejects anything that would
create a cycle.

```cpp
AudioApp::SampleSettings settings{ 256, 44100, 2 };
AudioApp::AudioEngine engine( settings );

auto osc  = engine.AddNode<AudioApp::Oscillator>( "Osc", 440, 1.0, AudioApp::sine );
auto gain = engine.AddNode<Gain>( "Gain", 0.5f );

engine.Connect( osc, gain );
engine.ConnectToOutput( gain );
```

Then hand the engine to a PortAudio callback and call `Nodal_Process()`.

## Real-Time Design

The audio callback does no allocation, no locking, and no sorting. The
topological order is computed on the control thread whenever the graph changes,
written into whichever of two slots is not currently live, then published with a
release store on an atomic index. The callback does one acquire load and walks a
flat vector. Biquad coefficients are published the same way — the whole
coefficient set flips at once rather than each coefficient being independently
atomic, which would let the filter briefly run on a mix of two configurations.

The double-buffer scheme assumes at most one publish per callback period. Two
graph edits or two coefficient updates inside a single buffer can land on the
slot the audio thread is holding. For UI-rate parameter changes this is fine;
batching the setters behind a single commit, or moving to a seqlock, is the fix
if it needs to hold under faster updates.

## Layout

`include/` headers · `src/` implementations · `Notebooks/` Python prototyping

## Building

Requires PortAudio. `PA_PREFIX` in the Makefile points at a Homebrew install and
will need adjusting for other setups.

```
brew install portaudio
make
./demo
```

## Results


