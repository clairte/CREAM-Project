#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

CREAMProjectAudioProcessorEditor::CREAMProjectAudioProcessorEditor (CREAMProjectAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, osc(audioProcessor.apvts,"OSC1WAVETYPE", "OSC1FMFREQ", "OSC1FMDEPTH")
, adsr("Amp Envelope", audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE")
, filter(audioProcessor.apvts, "FILTERTYPE", "FILTERCUTOFF", "FILTERRES")
, modAdsr("Mod Envelope", audioProcessor.apvts, "MODATTACK", "MODDECAY", "MODSUSTAIN", "MODRELEASE")
, presetPanel(p.getPresetManager()) //pass preset manager from audio processor 

{
    //setResizable(true, true);
    setSize (620, 500);

    addAndMakeVisible(adsr);
    addAndMakeVisible(osc);
    addAndMakeVisible(filter);
    addAndMakeVisible(modAdsr);
    addAndMakeVisible(presetPanel); 
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
    const int paddingY = 55;
    const int paddingY2 = 255;
    const int width = 300;
    const int height = 200;
    
    osc.setBounds (paddingX, paddingY, width, height);
    adsr.setBounds (osc.getRight(), paddingY, width, height);
    filter.setBounds(paddingX, paddingY2, width, height);
    modAdsr.setBounds(filter.getRight(), paddingY2, width, height);
    presetPanel.setBounds(getLocalBounds().removeFromTop(proportionOfHeight(0.1f)));
}


