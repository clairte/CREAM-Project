#include "ReverbSlider.h"

ReverbSlider::ReverbSlider(juce::AudioProcessorValueTreeState& apvts, juce::String reverbSliderId)
: reverbSlider("Air", reverbSliderId, apvts, sliderWidth, sliderHeight, juce::Slider::SliderStyle::LinearVertical)
{
    addAndMakeVisible(reverbSlider);
}

ReverbSlider::~ReverbSlider()
{
}

void ReverbSlider::resized()
{
    const auto startX = 30;
    const auto startY = 55;
    const auto width = sliderWidth;
    const auto height = sliderHeight + 20;
    
    reverbSlider.setBounds(startX, startY, width, height);
}
