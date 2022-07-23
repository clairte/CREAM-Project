#pragma once

#include <JuceHeader.h>

namespace Utility
{
    class ParameterHelper
    {
    public:
        ParameterHelper() = delete;

        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
        {
            //A vector of parameters
            std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
            
            //Oscillator select types
            params.push_back(std::make_unique<juce::AudioParameterChoice> ("OSC1TYPE", "Osc 1", juce::StringArray{"Sine", "Saw", "Square"}, 0));
            params.push_back(std::make_unique<juce::AudioParameterChoice> ("OSC2TYPE", "Osc 2", juce::StringArray{"Sine", "Saw", "Square"}, 0));
            
            //OSC Gain
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC1GAIN", "Osc 1 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f}, 0.1f, "dB"));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC2GAIN", "Osc 2 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f}, 0.1f, "dB"));
            
            //OSC Pitch Value
            params.push_back (std::make_unique<juce::AudioParameterInt> ("OSC1PITCH", "Osc 1 Pitch", -48, 48, 0));
            params.push_back (std::make_unique<juce::AudioParameterInt> ("OSC2PITCH", "Osc 2 Pitch", -48, 48, 0));
            
            //FM FREQ
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC1FMFREQ", "Osc 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f}, 0.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC2FMFREQ", "Osc 2 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f}, 0.0f));
            
            //FM DEPTH
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC1FMDEPTH", "Osc 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC2FMDEPTH", "Osc 2 FM Depth", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
            //kinda of the gain "how big you want this wave to be" if depth is 1, change main wave by +-1Hz, if wave is 200Hz, modulate by 199, 200, 201
            //default: sine wave going from -200 to 200, oscillating at rate of 5Hz
            
            //ADSR
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("ATTACK", "Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("DECAY", "Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 1.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("RELEASE", "Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f}, 0.4f));
            
            //Filter ADSR
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERADSRDEPTH", "Filter ADSR Depth", juce::NormalisableRange<float> { 0.1f, 10000.0f, 0.1f, 0.3f}, 10000.0f, ""));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERATTACK", "Filter Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERDECAY", "Filter Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERSUSTAIN", "Filter Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 1.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERRELEASE", "FIlter Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f}, 0.4f));
            
            //Filter
            params.push_back(std::make_unique<juce::AudioParameterChoice> ("FILTERTYPE", "Filter Type", juce::StringArray{"Low-Pass", "Band-Pass", "High-Pass"}, 0));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERCUTOFF", "Filter Cutoff", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 200.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERRES", "Filter Resonance", juce::NormalisableRange<float> { 1.0f, 10.0f, 0.1f}, 1.0f));
            
            //LFO
            params.push_back (std::make_unique<juce::AudioParameterFloat> ("LFO1FREQ", "LFO1 Frequency", juce::NormalisableRange<float> { 0.0f, 20.0f, 0.1f }, 0.0f, "Hz"));
            params.push_back (std::make_unique<juce::AudioParameterFloat> ("LFO1DEPTH", "LFO1 Depth", juce::NormalisableRange<float> { 0.0f, 10000.0f, 0.1f, 0.3f }, 0.0f, ""));
            
            //Reverb
            params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBSIZE", "Reverb Size", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
            params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBWIDTH", "Reverb Width", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f, ""));
            params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDAMPING", "Reverb Damping", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.5f, ""));
            params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDRY", "Reverb Dry", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f, ""));
            params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBWET", "Reverb Wet", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
            params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBFREEZE", "Reverb Freeze", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
            
            return {params.begin(), params.end() };
        }
    };
}
