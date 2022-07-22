#pragma once

#include <JuceHeader.h>

#include "OscData.h"

class FilterData : public juce::dsp::StateVariableTPTFilter<float>
{
public:
    
    FilterData();
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    
    void updateParameters(const int filterType, const float frequency, const float resonance, const float modulator = 1.0f);
    void setParams (const int filterType, const float filterCutoff, const float filterResonance, const float modulator = 1.0f); //set to 1 means multiply cutoff by1, change nothing
    
    void processNextBlock (juce::AudioBuffer<float>& buffer);
    float processNextSample (int channel, float inputValue); 
    
    void resetAll();
private:
//    juce::dsp::StateVariableTPTFilter<float> filter;
    void selectFilterType (const int filterType);
    
    juce::dsp::Oscillator<float> lfo { [](float x) { return std::sin(x); }}; 
    bool isPrepared { false };
};
