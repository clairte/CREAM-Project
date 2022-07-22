#include "FilterData.h"

FilterData::FilterData()
{
    setType(juce::dsp::StateVariableTPTFilterType::lowpass); 
}

void FilterData::prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels)
{
    resetAll();
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
    
    prepare(spec);
    isPrepared = true;
}

void FilterData::setParams (const int filterType, const float filterCutoff, const float filterResonance, const float modulator)
{
    selectFilterType (filterType);
    setResonance (filterResonance);
    
    float frequency = filterCutoff * modulator;
    frequency = std::fmax(frequency, 20.0f); //limit frequency cutoff to 20hz (DON'T GO BELOW!!!), returns larger of two, 20 if modFreq goes below
    frequency = std::fmin(frequency, 20000.0f); //limit frequency cutoff to 20000hz
    
    setCutoffFrequency (frequency);
}

void FilterData::processNextBlock (juce::AudioBuffer<float>& buffer)
{
    jassert(isPrepared);
    
    juce::dsp::AudioBlock<float> block { buffer };
    process(juce::dsp::ProcessContextReplacing<float> (block));
}

float FilterData::processNextSample(int channel, float inputValue)
{
    return processSample (channel, inputValue);
}

void FilterData::selectFilterType (const int filterType)
{
    switch (filterType)
    {
        case 0:
            setType (juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
            
        case 1:
            setType (juce::dsp::StateVariableTPTFilterType::bandpass);
            break;
            
        case 2:
            setType (juce::dsp::StateVariableTPTFilterType::highpass);
            break;
            
        default:
            setType (juce::dsp::StateVariableTPTFilterType::lowpass);
            break;
    }
}

void FilterData::resetAll()
{
    reset();
    lfo.reset(); 
}

//void FilterData::updateParameters(const int filterType, const float frequency, const float resonance, const float modulator)
//{
//    //take in filter type
//    switch (filterType)
//    {
//        case 0:
//            filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
//            break;
//        case 1:
//            filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
//            break;
//        case 2:
//            filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
//            break;
//    }
//
//    float modFreq = frequency * modulator;
//    modFreq = std::fmax(modFreq, 20.0f); //limit frequency cutoff to 20hz (DON'T GO BELOW!!!), returns larger of two, 20 if modFreq goes below
//    modFreq = std::fmin(modFreq, 20000.0f); //limit frequency cutoff to 20000hz
//
//    //set frequency and resonance
//    filter.setCutoffFrequency(modFreq);
//    filter.setResonance(resonance);
//}
