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
            //Combobox: switch oscillator
            std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
            
            //OSC select
            params.push_back(std::make_unique<juce::AudioParameterChoice>("OSC1WAVETYPE", "Osc 1 Wave Type", juce::StringArray{"Sine", "Saw", "Square"}, 0));
            
            //FM
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC1FMFREQ", "Osc 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f}, 0.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("OSC1FMDEPTH", "Osc 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.01f, 0.3f }, 0.0f));
            //kinda of the gain "how big you want this wave to be" if depth is 1, change main wave by +-1Hz, if wave is 200Hz, modulate by 199, 200, 201
            //default: sine wave going from -200 to 200, oscillating at rate of 5Hz
            
            //ADSR
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("ATTACK", "Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("DECAY", "Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 1.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("RELEASE", "Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f}, 0.4f));
            
            //Filter ADSR
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("MODATTACK", "Mod Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("MODDECAY", "Mod Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 0.1f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("MODSUSTAIN", "Mod Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f}, 1.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("MODRELEASE", "Mod Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f}, 0.4f));
            
            //Filter
            params.push_back(std::make_unique<juce::AudioParameterChoice>("FILTERTYPE", "Filter Type", juce::StringArray{"Low-Pass", "Band-Pass", "High-Pass"}, 0));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERCUTOFF", "Filter Cutoff", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 200.0f));
            params.push_back(std::make_unique<juce::AudioParameterFloat> ("FILTERRES", "Filter Resonance", juce::NormalisableRange<float> { 1.0f, 10.0f, 0.1f}, 1.0f));
            
            return {params.begin(), params.end() };
        }
    };
}
