#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

CREAMProjectAudioProcessorEditor::CREAMProjectAudioProcessorEditor (CREAMProjectAudioProcessor& p)
: AudioProcessorEditor (&p)
, audioProcessor (p)
, osc1(audioProcessor.apvts, "OSC1TYPE", "OSC1GAIN", "OSC1PITCH", "OSC1FMFREQ", "OSC1FMDEPTH")
, osc2(audioProcessor.apvts, "OSC2TYPE", "OSC2GAIN", "OSC2PITCH", "OSC2FMFREQ", "OSC2FMDEPTH")
, adsr(audioProcessor.apvts, "ATTACK", "DECAY", "SUSTAIN", "RELEASE")
, filter(audioProcessor.apvts, "FILTERTYPE", "FILTERCUTOFF", "FILTERRES")
, filterAdsr(audioProcessor.apvts, "FILTERATTACK", "FILTERDECAY", "FILTERSUSTAIN", "FILTERRELEASE")
, lfo1(audioProcessor.apvts, "LFO1FREQ", "LFO1DEPTH")
, reverb(audioProcessor.apvts, "REVERBSIZE", "REVERBDAMPING", "REVERBWIDTH", "REVERBDRY", "REVERBWET", "REVERBFREEZE")
, filterSlider(audioProcessor.apvts, "FILTERSLIDERALL")
, reverbSlider(audioProcessor.apvts, "REVERBSLIDERALL")
, presetPanel(p.getPresetManager()) //pass preset manager from audio processor 

{
    setSize (1080, 800);

    addAndMakeVisible(osc1);
    addAndMakeVisible(osc2);
    addAndMakeVisible(filter);
    addAndMakeVisible(adsr);
    addAndMakeVisible(filterAdsr);
    addAndMakeVisible(lfo1);
    addAndMakeVisible(reverb);
    
    addAndMakeVisible(filterSlider);
    addAndMakeVisible(reverbSlider);
    
    addAndMakeVisible(presetPanel);
    
    osc1.setName("Oscillator 1");
    osc2.setName("Oscillator 2");
    adsr.setName("ADSR");
    filter.setName("Filter");
    filterAdsr.setName("Filter ADSR");
    lfo1.setName("Filter LFO");
    
    filterSlider.setName("Filter Slider");
    reverbSlider.setName("Reverb Slider"); 
    
    auto oscColour = juce::Colour::fromRGB(247, 190, 67);
    auto filterColour = juce::Colour::fromRGB(246, 87, 64);
    
    osc1.setBoundsColour(oscColour);
    osc2.setBoundsColour(oscColour);
    
    filterAdsr.setBoundsColour(filterColour);
    filter.setBoundsColour(filterColour);
    lfo1.setBoundsColour(filterColour);
    
    filterSlider.setBoundsColour(filterColour);
    
    
    startTimerHz(30);
    
}

CREAMProjectAudioProcessorEditor::~CREAMProjectAudioProcessorEditor()
{
    stopTimer();
}

void CREAMProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void CREAMProjectAudioProcessorEditor::resized()
{
    
    const int oscWidth = 420;
    const auto oscHeight = 180;
    
    presetPanel.setBounds(getLocalBounds().removeFromTop(proportionOfHeight(0.1f)));
    
    osc1.setBounds(0, presetPanel.getBottom(), oscWidth, oscHeight);
    osc2.setBounds(0, osc1.getBottom(), oscWidth, oscHeight);
    filter.setBounds(osc1.getRight(), presetPanel.getBottom(), 180, 200);
    lfo1.setBounds(osc2.getRight(), filter.getBottom(), 180, 160);
    filterAdsr.setBounds(filter.getRight(), presetPanel.getBottom(), 230, 360);
    adsr.setBounds(filterAdsr.getRight(), presetPanel.getBottom(), 230, 360);
    reverb.setBounds(0, osc2.getBottom(), oscWidth, 150);
    filterSlider.setBounds(reverb.getRight(), lfo1.getBottom(), 130, 360);
    reverbSlider.setBounds(filterSlider.getRight(), lfo1.getBottom(), 130, 360); 
    
    
}

void CREAMProjectAudioProcessorEditor::timerCallback()
{
    repaint();
}
