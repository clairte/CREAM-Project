#include "OscData.h"

void OscData::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    resetAll();
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;
    
    fmOsc.prepare(spec);
    prepare(spec);
}

void OscData::setWaveType(const int choice)
{
    switch (choice) {
        case 0:
            //Sine
            initialise ([](float x) { return std::sin (x); });
            break;
        case 1:
            //Saw
            initialise ([](float x) { return x / juce::MathConstants<float>::pi; });
            break;
        case 2:
            //Square
            initialise ([](float x) { return x <0.0f ? -1.0f : 1.0f; });
            break;
            
        default:
            jassertfalse; //Something went wrong
            break;
    }
}

void OscData::setWaveFrequency(const int midiNoteNumber)
{
    setFrequency (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber + lastPitch) + fmModulator);
    //fmMod value will be between 1 and -1, modulates frequency of main wave
    lastMidiNote = midiNoteNumber;
}


void OscData::setOscPitch (const int pitch)
{
    lastPitch = pitch;
    setFrequency (juce::MidiMessage::getMidiNoteInHertz ((lastMidiNote + lastPitch) + fmModulator));

}

void OscData::setFmOsc (const float depth, const float freq)
{
    fmOsc.setFrequency(freq); //get from value tree, user sets
    fmDepth = depth;
    auto currentFreq = juce::MidiMessage::getMidiNoteInHertz (lastMidiNote + lastPitch) + fmModulator;
    setFrequency (currentFreq >= 0 ? currentFreq : currentFreq * -1.0f);
    //if currentFreq>=0, set to cF, if not, *-1.0 and set that
    //update frequency when fm is modulated, originally set frequency only calls when you press a midi note
    
}

void OscData::setGain (const float levelInDecibels)
{
    gain.setGainDecibels (levelInDecibels);
}

void OscData::setParams (const int oscChoice, const float oscGain, const int oscPitch, const float fmFreq, const float fmDepth)
{
    setWaveType (oscChoice);
    setGain (oscGain);
    setOscPitch (oscPitch);
    setFmOsc (fmFreq, fmDepth);
}

void OscData::renderNextBlock(juce::dsp::AudioBlock<float>& audioBlock)
{
    jassert(audioBlock.getNumSamples() > 0);
    process (juce::dsp::ProcessContextReplacing<float> (audioBlock));
    gain.process(juce::dsp::ProcessContextReplacing<float> (audioBlock)); 
}

float OscData::processNextSample(float input)
{
    fmModulator = fmOsc.processSample(input) * fmDepth;
    return gain.processSample (processSample (input));
}

void OscData::resetAll()
{
    reset();
    fmOsc.reset();
    gain.reset();
}
