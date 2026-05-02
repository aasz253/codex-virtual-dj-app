#include <JuceHeader.h>
#include "PlaylistComponent.h"

PlaylistComponent::PlaylistComponent(DJAudioPlayer* _djAudioPlayer,AudioFormatManager & formatManagerToUse,
                                     AudioThumbnailCache & cacheToUse) :    djAudioPlayer(_djAudioPlayer),wave(formatManagerToUse, cacheToUse)
{
    // Define headers
    tableComponent.getHeader().addColumn("Playlist",1,200);
    tableComponent.getHeader().addColumn("Options",2,200);

    
    tableComponent.setModel(this); //
    
    //==============================================================
    /**Set up the callback from DJAudioPlayer**/
    djAudioPlayer->setFileLoadedCallback([this](const File& loadedFile){
        fileLoadedCallback(loadedFile); /**Callback function to update the table with the loaded file**/
    });
    
    addAndMakeVisible(eraseTable); //display eraseTable button
    addAndMakeVisible(wave); //display the wave rect
    
    eraseTable.onClick = [this] { erasePlaylist(); };// Define what happens when the eraseTable button is clicked
    startTimer(1000); //thumbNail speed
    
    eraseTable.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

}

PlaylistComponent::~PlaylistComponent()
{
    stopTimer(); // destruct timer
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    // draw an outline around the component
    g.drawRect (getLocalBounds(), 1);
    g.setColour (juce::Colours::lightgrey);

    // definitions for placeholder text
    g.setColour (juce::Colours::grey);
    g.setFont (18.0f);
    
    // Check if the songs array is empty
     if (songs.isEmpty())
     {
         // The playlist is empty, so display a message
         g.drawText("Playlist is empty", tableComponent.getBounds(), juce::Justification::centred, true);
         g.drawText("Drag&Drop files to play!", tableComponent.getBounds(), juce::Justification::centredBottom, true);
     }
}

void PlaylistComponent::resized()
{
    // variables
    double rowH = getHeight()/6;
    int buttonHeight = 40;
    //setting boundaries of components
    wave.setBounds(30, getHeight()/1.35, getWidth()-50, rowH); // wave
    tableComponent.setBounds(0, getHeight()/10.5, getWidth(), (rowH*2)+(rowH)); // table
    eraseTable.setBounds(0, 0, getWidth(), buttonHeight/1.5); // Erase button

}

int PlaylistComponent::getNumRows()
{
    return songs.size(); //return num of rows
};

void PlaylistComponent::paintRowBackground (Graphics & g,
                         int rowNumber,
                         int width,
                         int height,
                         bool rowIsSelected)
{
    if(rowNumber == rowSelectedIndex || rowIsSelected == true) //highlight row if selected
    {
        g.fillAll(Colours::orange);
    }
    else // unselected
    {
        g.fillAll(Colours::grey);
    }
};

void PlaylistComponent::paintCell (Graphics & g,
                int rowNumber,
                int columnId,
                int width,
                int height,
                bool rowIsSelected)
{
    auto fileName = songs[rowNumber].getFileName();
    g.drawText(fileName,
               2,
               0,
               width-4,
               height,
               Justification::centredLeft, true);
};

bool PlaylistComponent::isInterestedInFileDrag(const StringArray &files)
{
    /**Check if a file has been dragged**/
    return files.size() > 0;
}

void PlaylistComponent::filesDropped(const StringArray &files, int x, int y)
{
    //Allow the user to select multiple files
    for(const auto& filePath : files)
    {
        File chosenFile(filePath);
        if(chosenFile.existsAsFile())
        {
            //Load the file in the player
            djAudioPlayer -> loadFile(File{filePath});
            addAndMakeVisible(tableComponent); //display tableComponent
        }
        else
        {
            //If file can't be verified
            std::cout << "DeckGUI::filesDropped - File does not exist: " << chosenFile.getFullPathName() << std::endl;
        }
    }
}

Component* PlaylistComponent::refreshComponentForCell (int rowNumber,
                                    int columnId,
                                    bool isRowSelected,
                                    Component *existingComponentToUpdate)
{
    if(columnId == 2)
    {
        auto* cellComponent = dynamic_cast<ButtonCellComponent*>(existingComponentToUpdate);
        if (cellComponent == nullptr)
        {
            cellComponent = new ButtonCellComponent(); //constructor
            cellComponent->setOnButtonClicked([this, rowNumber](const juce::String& buttonId) {
                rowSelectedIndex = rowNumber; // care status of buttons within the rows
                if (songs.size() > rowNumber) { // Ensure the row number is valid for the songs array
                    /** If rowNumber is valid find songs[rowNumber] and save on songFile */
                    const juce::File& songFile = songs[rowNumber];
                    if (buttonId == "PLAY") {
                        /** Load the songFile into secondary array responsible to hold tracks within playlist */
                        djAudioPlayer->loadFile(songFile);
                        djAudioPlayer->start(); // Play the song corresponding to the row
                        wave.loadFile(songFile); // Load new wevform
                        //testing only
                        DBG("Playing song at row: " + juce::String(rowNumber) + " - " + songFile.getFileName());
                        //----------
                    } else if (buttonId == "PAUSE") {
                        djAudioPlayer->stop(); // Pause the currently playing track
                        //testing only
                        DBG("Pausing song at row: " + juce::String(rowNumber));
                        //-----------
                    } else if (buttonId == "DELETE") {
                        songs.remove(rowNumber); //remove selected element from songs array (primary array)
                        djAudioPlayer->loadedFiles.remove(rowNumber); //remove selected element from djAudioPlayer array tracker (secondary array)
                        djAudioPlayer->stop(); // Pause the currently playing track
                        wave.audioThumb.clear(); // clear waveform
                        tableComponent.updateContent(); // Refresh the table to reflect the deletion
                    }
                }
            });

        }
        return cellComponent; // return ops within contructor
    }
    return existingComponentToUpdate; // refresh component
};


void PlaylistComponent::buttonClicked(Button* button)
{
};

void PlaylistComponent::fileLoadedCallback(const Array<File>& loadedFiles)
{
    //process multiple files into the table
    for(const auto& loadedFile : loadedFiles)
    {
        songs.add(loadedFile);
    }
    tableComponent.updateContent(); //Refresh the table elements

}

void PlaylistComponent::erasePlaylist() {
    songs.clear(); // Clear the current playlist
    djAudioPlayer->loadedFiles.clear(); //clear djAudioPlayer array tracker
    wave.audioThumb.clear(); // clear waveform
    djAudioPlayer->stop(); // Pause the currently playing track
    tableComponent.updateContent(); // Refresh the table elements to reflect the new playlist
};

void PlaylistComponent::timerCallback()
{
    wave.setPositionRelative(djAudioPlayer->getPositionRelative()); //timer to track thumbNail
}




