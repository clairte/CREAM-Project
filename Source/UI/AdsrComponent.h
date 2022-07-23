#pragma once

#include <JuceHeader.h>
#include "UtilityComponents.h"

class AdsrComponent  : public CustomComponent
{
public:
    AdsrComponent(juce::AudioProcessorValueTreeState& apvts, juce::String attackId, juce::String decayId, juce::String sustainId, juce::String releaseId);
    ~AdsrComponent() override;

    void resized() override;

private:
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    SliderWithLabel attack;
    SliderWithLabel decay;
    SliderWithLabel sustain;
    SliderWithLabel release;
    
    static constexpr int sliderWidth = 50;
    static constexpr int sliderHeight = 260;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdsrComponent)
};
