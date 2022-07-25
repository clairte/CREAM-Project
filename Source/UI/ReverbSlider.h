#include <JuceHeader.h>
#include "UtilityComponents.h"

class ReverbSlider  : public CustomComponent
{
public:
    ReverbSlider(juce::AudioProcessorValueTreeState& apvts, juce::String reverbSliderId);
    ~ReverbSlider() override;

    void resized() override;
private:
    SliderWithLabel reverbSlider;
    
    static constexpr int sliderWidth = 70;
    static constexpr int sliderHeight = 260;
};

