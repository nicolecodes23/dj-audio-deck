
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
// The WaveformDisplay class is responsible for visually representing an audio file's waveform.

class WaveformDisplay    : public Component,
                           public ChangeListener
{
public:
    WaveformDisplay( AudioFormatManager & 	formatManagerToUse,
                    AudioThumbnailCache & 	cacheToUse );
    ~WaveformDisplay();

    void paint (Graphics&) override;
    void resized() override;

    void changeListenerCallback (ChangeBroadcaster *source) override; //waveform updates

    void loadURL(URL audioURL); //load audio file

    // set the relative position of the playhead
    void setPositionRelative(double pos); //set position of playhead
    
    void setPositionChangeCallback(std::function<void(double)>callback);
    
    void setWaveformColour(Colour newColour); //changes waveform color


private:
    AudioThumbnail audioThumb;
    bool fileLoaded; //tracks if file is loaded
    double position; // current position playhead
    
    //mouse interaction to adjust playback
    void mouseDown(const MouseEvent& event)override;
    void mouseDrag(const MouseEvent& event)override;
    
    std::function<void(double)> onPositionChange; //notify position updates
    
    Colour waveformColour = juce::Colour(0xFF4D3BB7);// default waveform color
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
