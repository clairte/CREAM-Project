#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/AdsrComponent.h"
#include "UI/OscComponent.h"
#include "UI/FilterComponent.h"

//==============================================================================
/**
*/
class CREAMProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CREAMProjectAudioProcessorEditor (CREAMProjectAudioProcessor&);
    ~CREAMProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CREAMProjectAudioProcessor& audioProcessor;
    OscComponent osc;
    AdsrComponent adsr;
    FilterComponent filter;
    AdsrComponent modAdsr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CREAMProjectAudioProcessorEditor)
};
