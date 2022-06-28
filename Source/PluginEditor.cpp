#include "PluginProcessor.h"
#include "PluginEditor.h"

CREAMProjectAudioProcessorEditor::CREAMProjectAudioProcessorEditor (CREAMProjectAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, osc(audioProcessor.apvts,"OSC1WAVETYPE", "OSC1FMFREQ", "OSC1FMDEPTH")
, adsr("Amp Envelope", audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE")
, filter(audioProcessor.apvts, "FILTERTYPE", "FILTERCUTOFF", "FILTERRES")
, modAdsr("Mod Envelope", audioProcessor.apvts, "MODATTACK", "MODDECAY", "MODSUSTAIN", "MODRELEASE")

{
    setSize (620, 500);

    addAndMakeVisible(adsr);
    addAndMakeVisible(osc);
    addAndMakeVisible(filter);
    addAndMakeVisible(modAdsr);
}

CREAMProjectAudioProcessorEditor::~CREAMProjectAudioProcessorEditor()
{
}

void CREAMProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void CREAMProjectAudioProcessorEditor::resized()
{
    //set adsr bounds
    const int paddingX = 5;
    const int paddingY = 35;
    const int paddingY2 = 235;
    const int width = 300;
    const int height = 200;
        
    osc.setBounds (paddingX, paddingY, width, height);
    adsr.setBounds (osc.getRight(), paddingY, width, height);
    filter.setBounds(paddingX, paddingY2, width, height);
    modAdsr.setBounds(filter.getRight(), paddingY2, width, height);
}


