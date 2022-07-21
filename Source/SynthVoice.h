#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "Data/AdsrData.h"
#include "Data/OscData.h"
#include "Data/FilterData.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newPitchWheelValue) override;
    void controllerMoved (int controllerNumber, int newControllerValue) override;
    
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    void renderNextBlock (juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples) override;
    
    void updateAdsr(const float attack, const float decay, const float sustain, const float release);
    void updateModParams (const int filterType, const float filterCutoff, const float filterResonance, const float adsrDepth, const float lfoFreq, const float lfoDepth);
    void updateModAdsr (const float attack, const float decay, const float sustain, const float release);
    
    OscData& getOscillator() { return osc;}
    
    void reset();
    
private:
    juce::AudioBuffer<float> synthBuffer;
    static constexpr int numChannelsToProcess { 2 };

    OscData osc;
    AdsrData adsr;
    std::array<FilterData, numChannelsToProcess> filter;
    AdsrData filterAdsr;
    
    float filterAdsrOutput { 0.0f };
    
    juce::dsp::Gain<float> gain;
    bool isPrepared { false };
};
