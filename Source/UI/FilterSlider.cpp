#include "FilterSlider.h"

FilterSlider::FilterSlider(juce::AudioProcessorValueTreeState& apvts, juce::String filterSliderId)
: filterSlider("Warmth", filterSliderId, apvts, sliderWidth, sliderHeight, juce::Slider::SliderStyle::LinearVertical)
{
    addAndMakeVisible(filterSlider);
}

FilterSlider::~FilterSlider()
{
}

void FilterSlider::resized()
{
    const auto startX = 30;
    const auto startY = 55;
    const auto width = sliderWidth;
    const auto height = sliderHeight + 20;
    
    filterSlider.setBounds(startX, startY, width, height);
}
