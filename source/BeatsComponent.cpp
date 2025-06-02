

#include "BeatsComponent.h"

// Constructor
BeatsComponent::BeatsComponent()
{
    addAndMakeVisible(changeColoursButton);
    addAndMakeVisible(syncBeatsButton);

    // Set consistent button color
    auto buttonColor = juce::Colour(0xFF4D4364); // Dark grayish-purple
    changeColoursButton.setColour(TextButton::buttonColourId, buttonColor);
    syncBeatsButton.setColour(TextButton::buttonColourId, buttonColor);

    // Define button actions
    changeColoursButton.onClick = [this]() {
        if (onColourChange)
            onColourChange(generateRandomColour()); //Generate random color
    };

    syncBeatsButton.onClick = [this]() { syncBeats(); }; // Sync beats
}

// Destructor
BeatsComponent::~BeatsComponent() {}

// Paint function: Handles component drawing (fills background and displays BPM text)
void BeatsComponent::paint(Graphics& g)
{
    g.fillAll(juce::Colour(0xFF211939)); // Background color (Dark Blue-Grey)

    g.setColour(juce::Colour(0xFFE72BE8)); // Rectangles (Purple color)
    g.fillRect(leftRectangle);
    g.fillRect(rightRectangle);
    
    // Set text properties for BPM display
    g.setColour(Colours::white);
    g.setFont(Font(20.0f));

    g.drawFittedText(leftDeckBPM, leftRectangle, Justification::centred, 2);
    g.drawFittedText(rightDeckBPM, rightRectangle, Justification::centred, 2);
}

//Sets up component layout dynamically
void BeatsComponent::resized()
{
    auto bounds = getLocalBounds();
    int buttonHeight = 60;
    int rectWidth = getWidth() / 3;
    int rectHeight = getHeight() * 0.6;
    int spacing = (getWidth() - (rectWidth * 2)) / 3;

    // Position buttons
    changeColoursButton.setBounds(bounds.removeFromTop(buttonHeight).reduced(10));
    syncBeatsButton.setBounds(bounds.removeFromBottom(buttonHeight).reduced(10));

    // Position rectangles to visually represent deck BPMs
    leftRectangle.setBounds(spacing, buttonHeight + 10, rectWidth, rectHeight);
    rightRectangle.setBounds(spacing * 2 + rectWidth, buttonHeight + 10, rectWidth, rectHeight);
}

// Generates a random color
Colour BeatsComponent::generateRandomColour()
{
    return Colour::fromRGB(rand() % 256, rand() % 256, rand() % 256);
}

// Updates BPM text for a given deck and refreshes the UI
void BeatsComponent::updateBPM(int deck, double bpm) {
    if (deck == 1) {
        leftDeckBPM = "BPM: " + String(bpm);
    } else if (deck == 2) {
        rightDeckBPM = "BPM: " + String(bpm);
    }
    repaint(); //Triggers UI refresh to reflect changes
}

// Assigns deck references (required for syncing BPM)
void BeatsComponent::setDecks(DeckGUI* d1, DeckGUI* d2)
{
    deck1 = d1;
    deck2 = d2;
}

// Syncs beats between two decks by averaging their BPM
void BeatsComponent::syncBeats()
{
    if (!deck1 || !deck2) return; //Ensure decks exist before proceeding

    double bpm1 = deck1->getPlayer()->getBPM();
    double bpm2 = deck2->getPlayer()->getBPM();

    if (bpm1 <= 0 || bpm2 <= 0) return; //Prevents invalid BPM calculations

    double avgBPM = (bpm1 + bpm2) / 2.0; //Compute the average BPM

    // Adjust playback speed based on new BPM
    deck1->getPlayer()->setSpeed(avgBPM / bpm1);
    deck2->getPlayer()->setSpeed(avgBPM / bpm2);

    // Update BPM UI text
    leftDeckBPM = "BPM: " + String(avgBPM);
    rightDeckBPM = "BPM: " + String(avgBPM);
    repaint(); //Refresh UI
}
