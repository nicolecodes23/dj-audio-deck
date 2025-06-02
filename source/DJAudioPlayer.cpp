

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) 
: formatManager(_formatManager)
{

}
DJAudioPlayer::~DJAudioPlayer()
{

}

// Prepares the player for playback by initializing all audio sources
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    lowSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    midSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    highSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// Fills the audio buffer with the next block of audio samples
void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    highSource.getNextAudioBlock(bufferToFill);
}

// Releases audio resources when playback stops
void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

// Loads an audio file from a URL and prepares it for playback
void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {       
        std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, 
true)); 
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);             
        readerSource.reset (newSource.release());
        
        bpm = calculateBPM(); // Calculate BPM when a new track is loaded

    }
}

// Sets playback volume (ensures gain is within valid range)
void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 1.0)
    {
        std::cout << "DJAudioPlayer::setGain gain should be between 0 and 1" << std::endl;
    }
    else {
        transportSource.setGain(gain);
    }
   
}

void DJAudioPlayer::setSpeed(double ratio)
{
  if (ratio < 0 || ratio > 100.0)
    {
        std::cout << "DJAudioPlayer::setSpeed ratio should be between 0 and 100" << std::endl;
    }
    else {
        resampleSource.setResamplingRatio(ratio);
    }
}

// Moves playhead to a specific position in seconds
void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

// Moves playhead to a relative position (0.0 - 1.0)
void DJAudioPlayer::setPositionRelative(double pos)
{
     if (pos < 0 || pos > 1.0)
    {
        std::cout << "DJAudioPlayer::setPositionRelative pos should be between 0 and 1" << std::endl;
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start()
{
    transportSource.start();
}

void DJAudioPlayer::stop()
{
  transportSource.stop();
}

// Returns the playhead's relative position
double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

//check if track finished
bool DJAudioPlayer::isTrackFinished()
{
    return transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds();
}

//display bass frequencies
void DJAudioPlayer::setLowShelf(double gainFactor)
{
    IIRCoefficients lowShelf = IIRCoefficients::makeLowShelf(44100, 300, 1.0 / MathConstants<double>::sqrt2, gainFactor);
    lowSource.setCoefficients(lowShelf);
}

//display mid frequencies
void DJAudioPlayer::setPeakFilter(double gainFactor)
{
    IIRCoefficients peakFilter = IIRCoefficients::makePeakFilter(44100, 3000, 1.0 / MathConstants<double>::sqrt2, gainFactor);
    midSource.setCoefficients(peakFilter);
}

//display high frequencies
void DJAudioPlayer::setHighShelf(double gainFactor)
{
    IIRCoefficients highShelf = IIRCoefficients::makeHighShelf(44100, 4500, 1.0 / MathConstants<double>::sqrt2, gainFactor);
    highSource.setCoefficients(highShelf);
}

//calculate BPM
double DJAudioPlayer::calculateBPM() {
    if (!readerSource) return 0.0; // Ensure a track is loaded

    AudioBuffer<float> buffer(readerSource->getAudioFormatReader()->numChannels,
                              static_cast<int>(readerSource->getAudioFormatReader()->lengthInSamples));
    
    readerSource->getAudioFormatReader()->read(&buffer, 0, buffer.getNumSamples(), 0, true, true);
    
    int sampleRate = static_cast<int>(readerSource->getAudioFormatReader()->sampleRate);
    int numSamples = buffer.getNumSamples();
    
    // Convert to mono if stereo
    if (buffer.getNumChannels() > 1)
    {
        for (int i = 0; i < numSamples; i++)
        {
            float left = buffer.getSample(0, i);
            float right = buffer.getSample(1, i);
            buffer.setSample(0, i, (left + right) * 0.5f); // Average channels
        }
    }

    // Compute short-term energy
    std::vector<float> energyLevels;
    int windowSize = sampleRate / 100; // 10ms window
    for (int i = 0; i < numSamples - windowSize; i += windowSize)
    {
        float energy = 0.0f;
        for (int j = 0; j < windowSize; ++j) {
            float sample = buffer.getSample(0, i + j);
            energy += sample * sample;
        }
        energyLevels.push_back(energy);
    }

    // Peak detection
    std::vector<int> peaks;
    for (size_t i = 1; i < energyLevels.size() - 1; ++i)
    {
        if (energyLevels[i] > energyLevels[i - 1] && energyLevels[i] > energyLevels[i + 1])
            peaks.push_back(i);
    }

    if (peaks.size() < 2) return 0.0; // No beats detected

    // Calculate time intervals between peaks
    std::vector<float> intervals;
    for (size_t i = 1; i < peaks.size(); ++i)
    {
        float timeDiff = (peaks[i] - peaks[i - 1]) * (windowSize / static_cast<float>(sampleRate));
        intervals.push_back(timeDiff);
    }

    // Compute average interval and convert to BPM
    float avgInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0f) / intervals.size();
    bpm = 60.0f / avgInterval;

    return bpm;
}


