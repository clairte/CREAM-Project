#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/AdsrComponent.h"
#include "UI/OscComponent.h"
#include "UI/FilterComponent.h"
#include "UI/PresetPanel.h"
#include "UI/LfoComponent.h"
#include "UI/ReverbComponent.h"
#include "UI/FilterSlider.h"
#include "UI/ReverbSlider.h"

class CREAMProjectAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    CREAMProjectAudioProcessorEditor (CREAMProjectAudioProcessor&);
    ~CREAMProjectAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    CREAMProjectAudioProcessor& audioProcessor;

    OscComponent osc1;
    OscComponent osc2;
    AdsrComponent adsr;
    FilterComponent filter;
    AdsrComponent filterAdsr;
    LfoComponent lfo1;
    ReverbComponent reverb;
    
    FilterSlider filterSlider;
    ReverbSlider reverbSlider; 
    
    PresetPanel presetPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CREAMProjectAudioProcessorEditor)
};
