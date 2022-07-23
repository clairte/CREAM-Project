#pragma once

#include <JuceHeader.h>
#include "UtilityComponents.h"

class FilterComponent  : public CustomComponent
{
public:
    FilterComponent(juce::AudioProcessorValueTreeState& apvts, juce::String filterTypeId, juce::String filterCutoffId, juce::String filterResId);
    ~FilterComponent() override;

    void resized() override;

private:
    juce::ComboBox filterTypeSelector { "Filter Type" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeSelectorAttachment;
    
    SliderWithLabel cutoff;
    SliderWithLabel resonance;
    
    static constexpr int dialWidth = 70;
    static constexpr int dialHeight = 70;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterComponent)
};
