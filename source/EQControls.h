
#pragma once
#include <JuceHeader.h>

// EQControls class manages three frequency band sliders (Low, Mid, High)

class EQControls : public juce::Component
{
public:
    EQControls();  // Constructor: Initializes sliders and labels
    ~EQControls() override;  // Destructor: Cleans up resources (currently empty)

    void resized() override; // Arranges UI components when resized
    
    // Getter functions for accessing the sliders from outside the class
    juce::Slider& getLowSlider() { return lowSlider; }
    juce::Slider& getMidSlider() { return midSlider; }
    juce::Slider& getHighSlider() { return highSlider; }
        
    // Sliders for controlling different EQ frequency bands
    juce::Slider lowSlider;
    juce::Slider midSlider;
    juce::Slider highSlider;
    
    // Labels to indicate the function of each slider
    juce::Label lowLabel;
    juce::Label midLabel;
    juce::Label highLabel;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQControls) 
};
