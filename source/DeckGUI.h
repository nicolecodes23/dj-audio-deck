
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "EQControls.h"
#include "BeatsComponent.h"

//==============================================================================
/*
*/

class BeatsComponent; //prevent circular dependency

//DeckGUI provides graphical interface for controlling music playback
class DeckGUI    : public Component,
                   public Button::Listener, 
                   public Slider::Listener, 
                   public FileDragAndDropTarget, 
                   public Timer
{
public:
    DeckGUI(DJAudioPlayer* player, 
           AudioFormatManager & formatManagerToUse,
           AudioThumbnailCache & cacheToUse, int deckNumber );
    ~DeckGUI();

    void paint (Graphics&) override;
    void resized() override;

     //implement Button::Listener
    void buttonClicked (Button *) override;

    //implement Slider::Listener
    void sliderValueChanged (Slider *slider) override;

    //drag and drop file
    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y) override; 

    //uses certain UI periodically
    void timerCallback() override;
    
    //loads audio into deck
    void loadTrack(File file);
    
    //updates waveform position
    void positionChangedByWaveform(double pos);
    
    //retrieves waveform display
    WaveformDisplay& getWaveformDisplay() { return waveformDisplay; }

    //links to beat component to get BPM
    void setBeatsComponent(BeatsComponent* beatsComp) { beatsComponent = beatsComp; }
    
    DJAudioPlayer* getPlayer() { return player; }


    

private:
    
    juce::FileChooser fChooser{"Select a file..."};//file chooser
    
    String currentTrackTitle = "No track loaded";
    Label trackTitleLabel;

    TextButton playButton{"PLAY"};
    TextButton stopButton{"STOP"};
    TextButton loopButton{"LOOP"};
    
    bool isPlaying = false;
    bool isLooping = false;
    
    void checkLoopCondition(); //checks if looping applied
    
    void handleTrackCompletion(); //handles playback when track ends
      
    Slider volSlider; 
    Slider speedSlider;
    Slider posSlider;
    
    Slider& lowSlider;
    Slider& midSlider;
    Slider& highSlider;

    WaveformDisplay waveformDisplay;

    DJAudioPlayer* player; //reference to audio player
    
    EQControls eqControls;
    
    BeatsComponent* beatsComponent = nullptr; //reference to beat sync
    int deckID; //deck identifier 


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
