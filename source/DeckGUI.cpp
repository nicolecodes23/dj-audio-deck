
#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                AudioFormatManager & formatManagerToUse,
                AudioThumbnailCache & cacheToUse, int deckNumber)
                : lowSlider(eqControls.getLowSlider()),
                midSlider(eqControls.getMidSlider()),
                highSlider(eqControls.getHighSlider()),
                waveformDisplay(formatManagerToUse, cacheToUse),
                player(_player),
                deckID(deckNumber)
{
    //playback control buttons
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loopButton);
    
    //set button colors
    playButton.setColour(TextButton::buttonColourId, juce::Colour(0xFF4D4364));
    stopButton.setColour(TextButton::buttonColourId, juce::Colour(0xFF4D4364));
    loopButton.setColour(TextButton::buttonColourId, juce::Colour(0xFF4D4364));

    //sliders for volume,speed and position
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    
    //slider colors
    volSlider.setColour(Slider::trackColourId, juce::Colour(0xFFA8ADAF));//Graytrack
    volSlider.setColour(Slider::thumbColourId, juce::Colour(0xFFE72BE8)); //Purple thumb
    speedSlider.setColour(Slider::trackColourId, juce::Colour(0xFFA8ADAF));  // Gray track
    speedSlider.setColour(Slider::thumbColourId, juce::Colour(0xFFE72BE8));  // Purple thumb
    posSlider.setColour(Slider::trackColourId, juce::Colour(0xFFA8ADAF));  // Gray track
    posSlider.setColour(Slider::thumbColourId, juce::Colour(0xFFE72BE8));  // Purple thumb

    //waveform display
    addAndMakeVisible(waveformDisplay);
    
    //eq controls
    addAndMakeVisible(eqControls);
    
    //labels
    addAndMakeVisible(trackTitleLabel);
    trackTitleLabel.setJustificationType(Justification::centred);
    trackTitleLabel.setFont(Font(16.0f, Font::bold));
    
    //add event listener
    lowSlider.addListener(this);
    midSlider.addListener(this);
    highSlider.addListener(this);

    playButton.addListener(this);
    stopButton.addListener(this);
    loopButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);

    //range for sliders
    volSlider.setRange(0.0, 1.0);
    speedSlider.setRange(0.0, 10.0);
    posSlider.setRange(0.0, 1.0);
    
    //set waveform interaction with track position
    waveformDisplay.setPositionChangeCallback([this](double pos) {
        positionChangedByWaveform(pos);
    });



    startTimer(500);


}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{
    
    g.fillAll(juce::Colour(0xFF211939));

    g.setColour (Colours::white);
    g.setFont (14.0f);

}

void DeckGUI::resized() {
    auto bounds = getLocalBounds();
    auto buttonArea = bounds.removeFromTop(50); // Row for Play, Stop, Loop
    
    double buttonWidth = bounds.getWidth() / 3;
    playButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    stopButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    loopButton.setBounds(buttonArea.reduced(5));
    
    // row for track title
    auto titleArea = bounds.removeFromTop(20);
    trackTitleLabel.setBounds(titleArea.reduced(0,2));

    //row for waveform display
    auto waveformArea = bounds.removeFromTop(90);
    waveformDisplay.setBounds(waveformArea.reduced(10));

    //row for 3 sliders
    auto sliderHeight = 35;
    volSlider.setBounds(bounds.removeFromTop(sliderHeight).reduced(10));
    speedSlider.setBounds(bounds.removeFromTop(sliderHeight).reduced(10));
    posSlider.setBounds(bounds.removeFromTop(sliderHeight).reduced(10));
    
    //row for eq controls
    eqControls.setBounds(-10, 240, getWidth() - 30, 150);

}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playButton)
    {
        if (!isPlaying) //Only restart if track is stopped
        {
            if (player->getPositionRelative() >= 1.0) //Only reset if track has finished
            {
                player->setPositionRelative(0.0);
            }
            player->start();
            isPlaying = true;
            
            if (beatsComponent)
            {
                beatsComponent->updateBPM(deckID, player->getBPM());
            }
        }

        playButton.setColour(TextButton::buttonColourId, isPlaying ? Colours::green : Colours::darkgrey);
        stopButton.setColour(TextButton::buttonColourId, Colours::darkgrey);
    }
    else if (button == &stopButton)
    {
        isPlaying = false;
        player->stop();
        
        stopButton.setColour(TextButton::buttonColourId, Colours::red);
        playButton.setColour(TextButton::buttonColourId, Colours::darkgrey);

        playButton.setEnabled(true); //Ensure Play button can be clicked again
    }
    else if (button == &loopButton)
    {
        isLooping = !isLooping;
        loopButton.setColour(TextButton::buttonColourId, isLooping ? juce::Colour(0xFF5FA5E3) : juce::Colour(0xFF4D4364));

    }
}


void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }

    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());  // Updates track position
        waveformDisplay.setPositionRelative(slider->getValue());
    }
    if (slider == &lowSlider)
        {
            player->setLowShelf(slider->getValue());
        }
     if (slider == &midSlider)
        {
            player->setPeakFilter(slider->getValue());
        }
     if (slider == &highSlider)
        {
            player->setHighShelf(slider->getValue());
        }
    
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
  std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
  return true; 
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
  std::cout << "DeckGUI::filesDropped" << std::endl;
  if (files.size() == 1)
  {
    player->loadURL(URL{File{files[0]}});
  }
}

void DeckGUI::timerCallback()
{
    double pos = player->getPositionRelative();
    waveformDisplay.setPositionRelative(pos); //Keep waveform in sync

    if (isLooping && pos >= 1.0) //Looping is enabled, restart track
    {
        player->setPositionRelative(0.0);
        player->start();
    }
    else if (!isLooping && pos >= 1.0) //Track ended, allow replay
    {
        handleTrackCompletion();
    }
}

void DeckGUI::loadTrack(File file)
{
    player->loadURL(URL{file});
    waveformDisplay.loadURL(URL{file});
    
    currentTrackTitle = file.getFileName();
    trackTitleLabel.setText(currentTrackTitle, dontSendNotification);
    
    if (beatsComponent)
    {
        beatsComponent->updateBPM(deckID, player->getBPM()); 

    }
}

void DeckGUI::positionChangedByWaveform(double pos)
{
    player->setPositionRelative(pos);
    posSlider.setValue(pos, dontSendNotification);
}

void DeckGUI::handleTrackCompletion()
{
    if (!isLooping) //Only stop if loop is not enabled
    {
        isPlaying = false;
        player->stop();
        
        //Reset track position ONLY if it reached the end
        if (player->getPositionRelative() >= 1.0)
        {
            player->setPositionRelative(0.0);
        }

        playButton.setColour(TextButton::buttonColourId, Colours::darkgrey);
        stopButton.setColour(TextButton::buttonColourId, Colours::red);
        playButton.setEnabled(true); //Ensure Play button can restart playback
    }
}




    

