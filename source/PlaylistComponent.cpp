

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI* deck1, DeckGUI* deck2)
    : deckGUI1(deck1), deckGUI2(deck2)
{
   
    tableComponent.getHeader().addColumn("Deck 1", 1, 100);
    tableComponent.getHeader().addColumn("Deck 2", 2, 100);
    tableComponent.getHeader().addColumn("Track Title", 3, 400);
    tableComponent.getHeader().addColumn("Duration", 4, 100);
    tableComponent.getHeader().addColumn("Delete", 5,100 );

    tableComponent.getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, juce::Colour(0xFFCDC1FF)); // Soft Lavender
    tableComponent.getHeader().setColour(juce::TableHeaderComponent::textColourId, juce::Colour(0xFF4D3BB7)); //Font to Deep Purple-Blue


    tableComponent.setModel(this);

    addAndMakeVisible(importButton);
    importButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF4D4364)); // Dark Purple-Gray
    importButton.addListener(this);
    
    addAndMakeVisible(searchBar);
    searchBar.setTextToShowWhenEmpty("Search...", Colours::grey);
    searchBar.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF211939)); // Dark purple-blue background
    searchBar.setColour(juce::TextEditor::textColourId, juce::Colours::white);

    searchBar.onTextChange = [this]() {
        filterTracks(searchBar.getText());
    };
    
    addAndMakeVisible(tableComponent);
    

}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint (juce::Graphics& g)
{
 
    g.fillAll(juce::Colour(0xFF211939)); // Dark Purple-Blue Background

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("PlaylistComponent", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void PlaylistComponent::resized()
{
    auto bounds = getLocalBounds();
    auto topArea = bounds.removeFromTop(40); // Space for button & search bar
    
    importButton.setBounds(topArea.removeFromLeft(150).reduced(5));  // Set button width
    searchBar.setBounds(topArea.reduced(5));  // Remaining space for search bar

    tableComponent.setBounds(bounds);
}


int PlaylistComponent::getNumRows()
{
    return filteredIndices.empty() ? trackTitles.size() : filteredIndices.size();
}


void PlaylistComponent::paintRowBackground(Graphics & g, int rowNumber, int width,                                          int height, bool rowIsSelected)
{
    if(rowIsSelected)
    {
        g.fillAll(juce::Colour(0xFFE72BE8)); // Bright Pink-Purple for selected row
    } else
    {
        g.fillAll(juce::Colour(0xFF211939)); 
    }
    
};


void PlaylistComponent::paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    int index = filteredIndices.empty() ? rowNumber : filteredIndices[rowNumber]; //filtered index

    if (columnId == 3) // Track Title Column
    {
        g.setColour(juce::Colours::white);
        g.drawText(trackTitles[index], 2, 0, width - 4, height, Justification::centredLeft, true);
    }
    else if (columnId == 4) // Duration Column
    {
        g.setColour(juce::Colours::white);
        String durationString = String(trackDurations[index], 2) + " s";
        g.drawText(durationString, 2, 0, width - 4, height, Justification::centredLeft, true);
    }
}



Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate)
{
    int index = filteredIndices.empty() ? rowNumber : filteredIndices[rowNumber]; //Adjust for filtered list

    if (columnId == 1 || columnId == 2) { // Deck 1 or Deck 2 buttons
        if (existingComponentToUpdate == nullptr) {
            TextButton* btn = new TextButton{"Load"};
            btn->addListener(this);
            btn->setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF4D4364)); // Dark Purple-Gray
            btn->setComponentID(std::to_string(index) + "_" + std::to_string(columnId));
            existingComponentToUpdate = btn;
        }
    }
    else if (columnId == 5) { // Delete button
        if (existingComponentToUpdate == nullptr) {
            TextButton* deleteBtn = new TextButton{"X"};
            deleteBtn->addListener(this);
            deleteBtn->setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF4D4364)); // Dark Purple-Gray
            deleteBtn->setComponentID("delete_" + std::to_string(index));
            existingComponentToUpdate = deleteBtn;
        }
    }
    return existingComponentToUpdate;
}



void PlaylistComponent::buttonClicked(Button* button)
{
    if (button == &importButton)
    {
        importTracks(); // Call new function to import multiple files
    }
    else
    {
        // track deletion and loading into decks
        String id = button->getComponentID();

        if (id.startsWith("delete_"))
        {
            int trackIndex = id.substring(7).getIntValue();
            deleteTrack(trackIndex);
        }
        else
        {
            StringArray tokens;
            tokens.addTokens(id, "_", "");
            int trackIndex = tokens[0].getIntValue();
            int deck = tokens[1].getIntValue();

            loadTrackToDeck(deck, trackIndex);
        }
    }
}

void PlaylistComponent::importTracks()
{
    auto fileChooserFlags = FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems;

    fileChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
    {
        Array<File> chosenFiles = chooser.getResults();
        
        for (auto& file : chosenFiles)
        {
            if (file.existsAsFile())
            {
                trackTitles.push_back(file.getFileName().toStdString());
                trackPaths.push_back(file.getFullPathName().toStdString());
                trackDurations.push_back(getAudioFileDuration(file));
            }
        }

        tableComponent.updateContent(); // Refresh the table
    });
}

void PlaylistComponent::loadTrackToDeck(int deck, int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < trackPaths.size()) //Use trackPaths, not trackTitles
    {
        File file(trackPaths[trackIndex]); //Use stored full path

        if (!file.existsAsFile())
        {
            std::cerr << "Error: File not found - " << file.getFullPathName() << std::endl;
            return;
        }

        if (deck == 1)
        {
            deckGUI1->loadTrack(file);
        }
        else if (deck == 2)
        {
            deckGUI2->loadTrack(file);
        }
        else
        {
            std::cerr << "Error: Invalid deck number " << deck << std::endl;
        }
    }
}


double PlaylistComponent::getAudioFileDuration(File file)
{
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        return reader->lengthInSamples / reader->sampleRate; //Return duration in seconds
    }
    return 0.0;
}


void PlaylistComponent::deleteTrack(int trackIndex)
{
    if (trackIndex >= 0 && trackIndex < trackTitles.size()) {
        trackTitles.erase(trackTitles.begin() + trackIndex);
        trackPaths.erase(trackPaths.begin() + trackIndex);
        trackDurations.erase(trackDurations.begin() + trackIndex);
        
        filterTracks(searchBar.getText()); //Reapply search filter after deletion
    }
}


void PlaylistComponent::filterTracks(const String& searchText)
{
    filteredIndices.clear();
    std::string searchStr = searchText.toStdString(); //Convert to std::string

    for (size_t i = 0; i < trackTitles.size(); ++i)
    {
        if (trackTitles[i].rfind(searchStr, 0) == 0) //Only match from the start
        {
            filteredIndices.push_back(i);
        }
    }

    tableComponent.updateContent();
}

