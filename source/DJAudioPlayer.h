
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//responsible for audio playback and extracting BPM for beats component
class DJAudioPlayer : public AudioSource {
  public:

    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL); //load audio file
    void setGain(double gain); //set playback volume
    void setSpeed(double ratio); //adjust speed
    void setPosition(double posInSecs);
    void setPositionRelative(double pos); //set track position as percentage
    

    void start();
    void stop();

    //get the relative position of the playhead
    double getPositionRelative();
    
    //check if track ends
    bool isTrackFinished();
    
    //eq controls
    void setLowShelf(double gainFactor); //low frequency
    void setPeakFilter(double gainFactor); //mid frequency
    void setHighShelf(double gainFactor); //high frequency
    
    //calculates bpm using peak detection
    double calculateBPM();
    //returns calculated BPM
    double getBPM() const { return bpm; }


private:
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource; 
    ResamplingAudioSource resampleSource{&transportSource, false, 2};
    
    IIRFilterAudioSource lowSource{&resampleSource, false};
    IIRFilterAudioSource midSource{&lowSource, false};
    IIRFilterAudioSource highSource{&midSource, false};
    
    double bpm = 0.0;


};




