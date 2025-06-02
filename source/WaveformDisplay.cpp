
#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager & 	formatManagerToUse,
                                 AudioThumbnailCache & 	cacheToUse) :
                                 audioThumb(1000, formatManagerToUse, cacheToUse), 
                                 fileLoaded(false), 
                                 position(0)
                          
{

  audioThumb.addChangeListener(this); //listen for waveform updates
}

//destructor
WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (Graphics& g)
{
    g.fillAll(juce::Colour(0xFFCDC1FF)); // Soft lavender background

    g.setColour (waveformColour);
    if(fileLoaded) // if track loaded
    {
        //draw waveform
      audioThumb.drawChannel(g,
        getLocalBounds(), 
        0, 
        audioThumb.getTotalLength(), 
        0, 
        1.0f
      );
      g.setColour(Colours::lightgreen);
      g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight(),2.0);
    }
    else 
    {
      g.setFont (20.0f);
      g.drawText ("File not loaded...", getLocalBounds(),
                  Justification::centred, true);   // draw some placeholder text

    }
}

void WaveformDisplay::resized()
{
   
}

// Loads an audio file from a URL and updates the waveform display
void WaveformDisplay::loadURL(URL audioURL)
{
  audioThumb.clear();
  fileLoaded  = audioThumb.setSource(new URLInputSource(audioURL));
  if (fileLoaded)
  {
    std::cout << "wfd: loaded! " << std::endl;
    repaint();
  }
  else {
    std::cout << "wfd: not loaded! " << std::endl;
  }

}

void WaveformDisplay::changeListenerCallback (ChangeBroadcaster *source)
{
    repaint();

}

// Updates the relative playback position and refreshes the display
void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}

// Sets a callback function for position changes (used by DeckGUI)
void WaveformDisplay::setPositionChangeCallback(std::function<void(double)> callback)
{
    onPositionChange = std::move(callback);
}


//updates playhead position based on mouse event
void WaveformDisplay::mouseDown(const MouseEvent& event)
{
    if (!fileLoaded) return;

    double clickPosition = event.position.x / getWidth(); //Convert click to relative position
    setPositionRelative(clickPosition);

    if (onPositionChange) //Notify DeckGUI to update track position
    {
        onPositionChange(clickPosition);
    }
}

// Handles mouse drag: Updates playhead position dynamically as the user drags
void WaveformDisplay::mouseDrag(const MouseEvent& event)
{
    if (!fileLoaded) return;

    double dragPosition = event.position.x / getWidth(); //Convert drag to relative position
    setPositionRelative(dragPosition);

    if (onPositionChange) //Notify DeckGUI
    {
        onPositionChange(dragPosition);
    }
}

void WaveformDisplay::setWaveformColour(Colour newColour)
{
    waveformColour = newColour;
    repaint();  //Force UI to refresh
}




