
#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "ButtonCellComponent.h"

#include <functional>

class PlaylistComponent  : public juce::Component,
                           public TableListBoxModel,
                           public Button::Listener,
                           public FileDragAndDropTarget,
                           public Timer
{
public:
    PlaylistComponent(DJAudioPlayer* _djAudioPlayer,
                      AudioFormatManager & formatManagerToUse,
                      AudioThumbnailCache & cacheToUse);
    ~PlaylistComponent() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    int getNumRows() override;
    void paintRowBackground (Graphics &,
                             int rowNumber,
                             int width,
                             int height,
                             bool rowIsSelected) override;
    
    void paintCell (Graphics &,
                    int rowNumber,
                    int columnId,
                    int width,
                    int height,
                    bool rowIsSelected) override;
    
    Component* refreshComponentForCell (int rowNumber,
                                        int columnId,
                                        bool isRowSelected,
                                        Component *existingComponentToUpdate) override;
    
    void buttonClicked(Button* button) override;
    
    void timerCallback() override;
    
    //drag and drop files
    bool isInterestedInFileDrag(const StringArray &files) override;
    void filesDropped(const StringArray &files, int x, int y) override;
    
    //==================================
    /**Function to handle file loaded notification from DJAudioPlayer**/
    void fileLoadedCallback(const Array<File>& loadedFiles);
    
    Array<File> songs; //Primary array
    
    void erasePlaylist(); //Erase files from playlist
    
    // Define a callback type for file loading
    using FileLoadCallback = std::function<void(const juce::File&)>;
    
    /**Check if row is selected to change colour**/
    int rowSelectedIndex = -1;
    
private:
    
    TextButton eraseTable{"CLEAR TABLE"}; // erase button

    AudioFormatManager formatManager;
    AudioTransportSource transportSource;
    
    TableListBox tableComponent; // instance
   
    DJAudioPlayer* djAudioPlayer; // instance
    
    WaveformDisplay wave; // instance
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};
