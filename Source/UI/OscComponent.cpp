#include <JuceHeader.h>
#include "OscComponent.h"

OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts, juce::String waveSelectorId, juce::String gainId, juce::String pitchId, juce::String fmFreqId, juce::String fmDepthId)
: gain("Gain", gainId, apvts, dialWidth, dialHeight)
, pitch("Pitch", pitchId, apvts, dialWidth, dialHeight)
, fmFreq("FM Freq", fmFreqId, apvts, dialWidth, dialHeight)
, fmDepth("FM Depth", fmDepthId, apvts, dialWidth, dialHeight)
{
    //Set up osc selector
    juce::StringArray choices {"Sine", "Saw", "Square"};
    oscSelector.addItemList(choices, 1);
    oscSelector.setSelectedItemIndex(0);
    addAndMakeVisible(oscSelector);
    
    oscSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, waveSelectorId, oscSelector);
    
    //Set up completed in utility components slider w label
    addAndMakeVisible(gain);
    addAndMakeVisible(pitch);
    addAndMakeVisible(fmFreq);
    addAndMakeVisible(fmDepth);
    
}

OscComponent::~OscComponent()
{
}

void OscComponent::resized()
{
    const int yStart = 15;
    const int width = 70;
    const int height = 88;
    
    oscSelector.setBounds(18, 40, 100, 25);
    gain.setBounds(120, yStart, width, height);
    pitch.setBounds(190, yStart, width, height);
    fmFreq.setBounds(260, yStart, width, height);
    fmDepth.setBounds(330, yStart, width, height);
    
}

