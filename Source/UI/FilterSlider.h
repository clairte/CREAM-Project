#include <JuceHeader.h>
#include "UtilityComponents.h"

class FilterSlider  : public CustomComponent
{
public:
    FilterSlider(juce::AudioProcessorValueTreeState& apvts, juce::String filterSliderId);
    ~FilterSlider() override;

    void resized() override;
private:
    SliderWithLabel filterSlider;
    
    static constexpr int sliderWidth = 70;
    static constexpr int sliderHeight = 260;
};

