#include "SynthVoice.h"

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    //osc.setWaveFrequency(midiNoteNumber);
    
    for (int i = 0; i < 2; i++)
    {
        osc1[i].setWaveFrequency (midiNoteNumber);
        osc2[i].setWaveFrequency (midiNoteNumber);
    }
    
    adsr.noteOn();
    filterAdsr.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    adsr.noteOff();
    filterAdsr.noteOff();
    if (! allowTailOff || !adsr.isActive())
    {
        clearCurrentNote();
    }
}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{
    
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    reset();
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
    
    for (int ch = 0; ch < numChannelsToProcess; ch++)
    {
        osc1[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        osc2[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        filter[ch].prepareToPlay (sampleRate, samplesPerBlock, outputChannels);
        lfo[ch].prepare (spec);
        lfo[ch].initialise ([](float x) { return std::sin (x); });
    }
    
    //osc.prepareToPlay(spec);
    //filter.prepareToPlay(sampleRate, samplesPerBlock, outputChannels);
    
    adsr.setSampleRate(sampleRate);
    filterAdsr.setSampleRate(sampleRate);
    
    gain.prepare(spec);
    gain.setGainLinear(0.3f); //linear instead of decibels [0,1]
    
    isPrepared = true;
}

void SynthVoice::renderNextBlock (juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples)
{
    jassert(isPrepared); //if is prepared not called, stop execution
    
    //if no voices active, just return
    if (! isVoiceActive()) { return; }
    
    //Set up
    synthBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    filterAdsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());
    filterAdsrOutput = filterAdsr.getNextSample(); 
    
    synthBuffer.clear();
    
    
    //Write osc data to buffer
    for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
    {
     auto* buffer = synthBuffer.getWritePointer (ch, 0);
     
        for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
        {
            buffer[s] = osc1[ch].processNextSample (buffer[s]) + osc2[ch].processNextSample (buffer[s]);
        }
    }
    
    //Creates an AudioBlock that points to the data in an AudioBuffer
    juce::dsp::AudioBlock<float> audioBlock (synthBuffer);
    //gain takes audioBlock values and returns gain turned down
    gain.process(juce::dsp::ProcessContextReplacing<float> (audioBlock));
    adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());
    
    //osc process runs, finish running, audioBlock contain sinewave info
    //osc.getNextAudioBlock(audioBlock);
    //filter.process(synthBuffer);
    
    //Write filter processing to buffer
    for (int ch = 0; ch < synthBuffer.getNumChannels(); ++ch)
    {
        auto* buffer = synthBuffer.getWritePointer(ch, 0);
        
        for (int s = 0; s < synthBuffer.getNumSamples(); ++s)
        {
            buffer[s] = filter[ch].processNextSample (ch, synthBuffer.getSample (ch, s));
        }
    }
    
    
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel )
    {
        outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);
        if (!adsr.isActive())
        {
            clearCurrentNote();
        }
    }
}

void SynthVoice::updateAdsr(const float attack, const float decay, const float sustain, const float release)
{
    adsr.updateADSR(attack, decay, sustain, release);
}


void SynthVoice::reset()
{
    gain.reset();
    adsr.reset();
    filterAdsr.reset();
}

//void SynthVoice::updateFilter(const int filterType, const float cutoff, const float resonance)
//{
//    float modulator = modAdsr.getNextSample();
//    filter.updateParameters(filterType, cutoff, resonance, modulator);
//}

void SynthVoice::updateModParams (const int filterType, const float filterCutoff, const float filterResonance, const float adsrDepth, const float lfoFreq, const float lfoDepth)
{
    auto cutoff = (adsrDepth * filterAdsrOutput) + filterCutoff;
        cutoff = std::clamp<float> (cutoff, 20.0f, 20000.0f);

    for (int ch = 0; ch < numChannelsToProcess; ++ch)
    {
        filter[ch].setParams (filterType, cutoff, filterResonance);
    }
}

void SynthVoice::updateModAdsr (const float attack, const float decay, const float sustain, const float release)
{
    filterAdsr.updateADSR (attack, decay, sustain, release);
}

    
