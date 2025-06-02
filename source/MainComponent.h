/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "BeatsComponent.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    


private:
    //==============================================================================
    // Your private member variables go here...
     
    //handles audio format
    AudioFormatManager formatManager;
    //cache for storing waveform thumbnails
    AudioThumbnailCache thumbCache{100};

    //deck1
    DJAudioPlayer player1{formatManager};
    DeckGUI deckGUI1{&player1, formatManager, thumbCache,1};

    //deck2
    DJAudioPlayer player2{formatManager};
    DeckGUI deckGUI2{&player2, formatManager, thumbCache,2};

    //mixer to combine audio from both decks
    MixerAudioSource mixerSource;
    
    //playlist managing both decks for loading
    PlaylistComponent playlistComponent{&deckGUI1, &deckGUI2};
    
    //controls beat synchronization
    BeatsComponent beatsComponent;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
