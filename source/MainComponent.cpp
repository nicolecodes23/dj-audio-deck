/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(): playlistComponent(&deckGUI1, &deckGUI2)
{
   
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }  
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }  

    addAndMakeVisible(deckGUI1); 
    addAndMakeVisible(deckGUI2);
    
    addAndMakeVisible(playlistComponent);

    addAndMakeVisible(beatsComponent);
    
    beatsComponent.onColourChange = [this](Colour newColour) {
        deckGUI1.getWaveformDisplay().setWaveformColour(newColour);
        deckGUI2.getWaveformDisplay().setWaveformColour(newColour);
    };
    
    deckGUI1.setBeatsComponent(&beatsComponent);
    deckGUI2.setBeatsComponent(&beatsComponent);
    
    beatsComponent.setDecks(&deckGUI1, &deckGUI2);

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);

 }
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

}

void MainComponent::resized()
{
    double deckWidth = getWidth() * 0.4;   // Reduce width to 40% each
    double beatsWidth = getWidth() * 0.2;  // Center component takes 20%
    double deckHeight = getHeight() * 0.65;
    double playlistHeight = getHeight() - deckHeight;

    deckGUI1.setBounds(0, 0, deckWidth, deckHeight);
    beatsComponent.setBounds(deckWidth, 0, beatsWidth, deckHeight);  // Centered
    deckGUI2.setBounds(deckWidth + beatsWidth, 0, deckWidth, deckHeight);
    playlistComponent.setBounds(0, deckHeight, getWidth(), playlistHeight);
}


