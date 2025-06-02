
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include "DeckGUI.h"

// The PlaylistComponent class handles the track listing, importing, searching, and managing playback.
class PlaylistComponent  : public juce::Component,
                           public TableListBoxModel,
                           public Button::Listener
{
public:
    PlaylistComponent(DeckGUI* deck1, DeckGUI* deck2); // Constructor
    ~PlaylistComponent() override; // Destructor

    void paint (juce::Graphics&) override;
    void resized() override; // Adjusts layout when resized
    
    int getNumRows() override; // Returns the number of tracks in the playlist
    
    void paintRowBackground(Graphics & g, int rowNumber, int width, int height, bool rowIsSelected) override; // Draws row background
    
    void paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override; // Draws track details
    
    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate) override; // Adds interactive buttons to table
    
    void buttonClicked(Button * button) override; // Handles button clicks
    
    void loadTrackToDeck(int deck, int trackIndex); // Loads a track into one of the decks
    void deleteTrack(int trackIndex); // Removes a track from the playlist

private:
    TableListBox tableComponent; // Table to display the playlist
    
    TextButton importButton{"Import Tracks"}; // Button to import tracks
    TextEditor searchBar; // Search bar for filtering tracks
    
    FileChooser fileChooser{"Select audio files...", File{}, "*.mp3;*.wav"}; // File picker for importing audio files
    
    DeckGUI* deckGUI1; // Reference to first deck
    DeckGUI* deckGUI2; // Reference to second deck
    
    void importTracks(); // Imports tracks from file chooser
    
    std::vector<std::string> trackTitles; // Stores track names
    std::vector<std::string> trackPaths; // Stores full file paths
    std::vector<double> trackDurations; // Stores track durations
    
    std::vector<int> filteredIndices; // Stores filtered track indices
    void filterTracks(const String& searchText); // Filters tracks based on search input
    
    double getAudioFileDuration(File file); // Retrieves duration of an audio file
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent) 
};
