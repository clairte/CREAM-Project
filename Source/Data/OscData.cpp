#include "OscData.h"

void OscData::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
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
    setFrequency (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber) + fmMod);
    //fmMod value will be between 1 and -1, modulates frequency of main wave
    lastMidiNote = midiNoteNumber;
}

void OscData::getNextAudioBlock(juce::dsp::AudioBlock<float>& block)
{
    for (int ch = 0; ch < block.getNumChannels(); ++ch)
    {
        //go into samples in our chennel
        for (int s= 0; s < block.getNumSamples(); ++s)
        {
            fmMod = fmOsc.processSample(block.getSample(ch, s)) * fmDepth;  //processSample returns a floatingpoint value
        }
    }
    process(juce::dsp::ProcessContextReplacing<float> (block));
}

void OscData::updateFm (const float depth, const float freq)
{
    fmOsc.setFrequency(freq); //get from value tree, user sets
    fmDepth = depth;
    auto currentFreq = juce::MidiMessage::getMidiNoteInHertz (lastMidiNote) + fmMod;
    setFrequency (currentFreq >= 0 ? currentFreq : currentFreq * -1.0f); //if currentFreq>=0, set to cF, if not, *-1.0 and set that
    //update frequency when fm is modulated, originally set frequency only calls when you press a midi note
    
}

