#include <JuceHeader.h>
#include "FilterComponent.h"

FilterComponent::FilterComponent(juce::AudioProcessorValueTreeState& apvts, juce::String filterTypeSelectorId, juce::String filterCutoffId, juce::String filterResId)
: cutoff("Cutoff", filterCutoffId, apvts, dialWidth, dialHeight)
, resonance("Resonance", filterResId, apvts, dialWidth, dialHeight)
{
    juce::StringArray filterTypeChoices {"Low-Pass", "Band-Pass", "High-Pass"};
    filterTypeSelector.addItemList(filterTypeChoices, 1);
    filterTypeSelector.setSelectedItemIndex(0);
    addAndMakeVisible(filterTypeSelector);
    
    filterTypeSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, filterTypeSelectorId, filterTypeSelector);
    
    addAndMakeVisible(cutoff);
    addAndMakeVisible(resonance);
}

FilterComponent::~FilterComponent()
{
}

void FilterComponent::resized()
{
    const int startX = 18;
    const int startY = 80;
    const int width = 70;
    const int height = 88;
    
    filterTypeSelector.setBounds(18, 40, 145, 25);
    cutoff.setBounds(startX, startY, width, height);
    resonance.setBounds(cutoff.getRight(), startY, width, height);
}
