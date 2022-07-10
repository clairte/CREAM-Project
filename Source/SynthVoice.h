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
    void updateFilter(const int filterType, const float cutoff, const float resonance);
    void updateModAdsr (const float attack, const float decay, const float sustain, const float release);
    
    OscData& getOscillator() { return osc;}
    
    void reset();
    
    std::array<OscData, 2>& getOscillator1() { return osc1; }
    std::array<OscData, 2>& getOscillator2() { return osc2; }
    AdsrData& getAdsr() { return adsr; }
    AdsrData& getFilterAdsr() { return filterAdsr; }
    float getFilterAdsrOutput() { return filterAdsrOutput; }
    void updateModParams (const int filterType, const float filterCutoff, const float filterResonance, const float adsrDepth, const float lfoFreq, const float lfoDepth);
    
private:
    OscData osc;
    AdsrData adsr;
    FilterData filter;
    AdsrData modAdsr;
    
  
    static constexpr int numChannelsToProcess { 2 };
    std::array<OscData, numChannelsToProcess> osc1;
    std::array<OscData, numChannelsToProcess> osc2;
    std::array<FilterData, numChannelsToProcess> filter;
    std::array<juce::dsp::Oscillator<float>, numChannelsToProcess> lfo;
    //adsr: turn the sound on and off.
    
    /*step 1, create an adsr object*/
    AdsrData adsr;
    AdsrData filterAdsr;
    juce::AudioBuffer<float> synthBuffer;
    float filterAdsrOutput { 0.0f };
    std::array<float, numChannelsToProcess> lfoOutput { 0.0f, 0.0f };

    
    juce::dsp::Gain<float> gain;
    bool isPrepared { false };
};
