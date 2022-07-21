#include "OscData.h"

void OscData::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    resetAll();
    
    //spec contains all necessary info (e.g. sample rate) passed down when called from synthVoice prepare to play
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

void OscData::getNextAudioBlock(juce::dsp::AudioBlock<float>& block)
{
    for (int ch = 0; ch < block.getNumChannels(); ++ch)
    {
        //go into samples in our chennel
        for (int s= 0; s < block.getNumSamples(); ++s)
        {
            fmModulator = fmOsc.processSample(block.getSample(ch, s)) * fmDepth;  //processSample returns a floatingpoint value
        }
    }
    process(juce::dsp::ProcessContextReplacing<float> (block));
}

void OscData::resetAll()
{
    reset();
    fmOsc.reset();
    gain.reset();
}
