#pragma once
#include <JuceHeader.h>

class OscData : public juce::dsp::Oscillator<float>
{
public:
    
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    void setWaveType(const int choice);
    void setWaveFrequency (const int midiNoteNumber);
    void setOscPitch (const int pitch);
    void setFmOsc (const float depth, const float freq);
    void setGain (const float levelInDecibels);
    void setParams (const int oscChoice, const float oscGain, const int oscPitch, const float fmFreq, const float fmDepth);
    
    void getNextAudioBlock(juce::dsp::AudioBlock<float>& block);
    void renderNextBlock(juce::dsp::AudioBlock<float>& audioBlock); 
    float processNextSample (float input);
    void resetAll();
    
    
private:
    
    juce::dsp::Oscillator<float> fmOsc { [](float x) { return std::sin (x); } };
    juce::dsp::Gain<float> gain;
    
    float fmModulator { 0.0f };
    float fmDepth { 0.0f };
    int lastMidiNote { 0 };
    int lastPitch { 0 };
};
