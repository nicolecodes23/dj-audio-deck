
#include "EQControls.h"

// Initializes sliders with styles, range, colors, and labels
EQControls::EQControls()
{
    // Initialize and configure sliders
    auto configureSlider = [](juce::Slider& slider) {
        slider.setSliderStyle(juce::Slider::Rotary);
        slider.setRange(0.1, 5.0, 0.1);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
        slider.setColour(juce::Slider::trackColourId, juce::Colour(0xFFA8ADAF)); // Track color (gray)
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFFE72BE8)); // Thumb color (pinkish)
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF4D3BB7)); // Fill color (deep purple-blue)
    };

    // Apply the same settings to all three sliders
    configureSlider(lowSlider);
    configureSlider(midSlider);
    configureSlider(highSlider);

    // Add sliders to the component
    addAndMakeVisible(lowSlider);
    addAndMakeVisible(midSlider);
    addAndMakeVisible(highSlider);

    // Initialize and configure labels
    auto configureLabel = [](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
    };

    // Apply settings to labels
    configureLabel(lowLabel, "Low");
    configureLabel(midLabel, "Mid");
    configureLabel(highLabel, "High");

    // Add labels to the component
    addAndMakeVisible(lowLabel);
    addAndMakeVisible(midLabel);
    addAndMakeVisible(highLabel);
}

// Destructor
EQControls::~EQControls() {}

// Arranges the layout of sliders and labels when resized
void EQControls::resized()
{
    // Set bounds for sliders
    lowSlider.setBounds(20, 30, 100, 100);
    midSlider.setBounds(110, 30, 100, 100);
    highSlider.setBounds(200, 30, 100, 100);

    // Position labels dynamically relative to sliders
    lowLabel.setBounds(lowSlider.getX(), lowSlider.getY() + 35, lowSlider.getWidth(), 20);
    midLabel.setBounds(midSlider.getX(), midSlider.getY() + 35, midSlider.getWidth(), 20);
    highLabel.setBounds(highSlider.getX(), highSlider.getY() + 35, highSlider.getWidth(), 20);
}
