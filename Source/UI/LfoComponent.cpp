#include <JuceHeader.h>

#include "LfoComponent.h"

LfoComponent::LfoComponent(juce::AudioProcessorValueTreeState& apvts, juce::String lfoFreqId, juce::String lfoDepthId)
: lfoFreq("LFO Freq", lfoFreqId, apvts, dialWidth, dialHeight)
, lfoDepth("LFO DEPTH", lfoDepthId, apvts, dialWidth, dialHeight)
{
    addAndMakeVisible(lfoFreq);
    addAndMakeVisible(lfoDepth);
}

LfoComponent::~LfoComponent()
{
    
}

void LfoComponent::resized()
{
    const int width = 70;
    const int height = 88;
    const int startY = 40;
    
    lfoFreq.setBounds(18, startY, width, height);
    lfoDepth.setBounds(90, startY, width, height);
    
}
