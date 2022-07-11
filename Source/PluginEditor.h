#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/AdsrComponent.h"
#include "UI/OscComponent.h"
#include "UI/FilterComponent.h"
#include "UI/PresetPanel.h"
#include "UI/LfoComponent.h"
#include "UI/ReverbComponent.h"
#include "UI/MeterComponent.h"
#include "UI/Assets.h"

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
    
    void timerCallback() override;

private:
    CREAMProjectAudioProcessor& audioProcessor;

    OscComponent osc;
    AdsrComponent adsr;
    FilterComponent filter;
    AdsrComponent modAdsr;
    ReverbComponent reverb;
    MeterComponent meter;
    juce::ImageComponent logo;
    
    PresetPanel presetPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CREAMProjectAudioProcessorEditor)
};
