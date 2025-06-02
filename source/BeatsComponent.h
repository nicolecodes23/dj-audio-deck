
#pragma once

#include <JuceHeader.h>
#include <random>
#include "DeckGUI.h"

class DeckGUI; // Avoid circular dependency

// The BeatsComponent class is responsible for handling beat synchronization
class BeatsComponent : public Component
{
public:
    BeatsComponent(); // Constructor
    ~BeatsComponent() override; // Destructor

    void paint(Graphics&) override; //draws BPM text and rectangles
    void resized() override; // Handles layout and positioning of elements when resized
    
    std::function<void(Colour)> onColourChange; // Function callback for color change events
    
    void updateBPM(int deck, double bpm); // Updates BPM text for the given deck
    void setDecks(DeckGUI* deck1, DeckGUI* deck2); // Sets deck references
    void syncBeats(); // Adjusts both decks to a synchronized BPM

private:
    // UI Components: Buttons for user interaction
    TextButton changeColoursButton{"Change Colours"};
    TextButton syncBeatsButton{"Sync Beats"};

    // Rectangles representing BPM decks 
    Rectangle<int> leftRectangle;
    Rectangle<int> rightRectangle;
    
    // Function to generate a random color for UI effects
    Colour generateRandomColour();
    
    // Pointers to the deck GUI components (used for retrieving BPM and adjusting speed)
    DeckGUI* deck1 = nullptr;
    DeckGUI* deck2 = nullptr;

    // Strings storing current BPM for UI display
    String leftDeckBPM = "No BPM";
    String rightDeckBPM = "No BPM";
};
