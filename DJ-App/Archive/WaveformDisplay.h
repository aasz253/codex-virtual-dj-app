#pragma once // Ensure this header file is only included once during compilation
#include <JuceHeader.h> // Include the JUCE framework header files

class WaveformDisplay  : public Component, // Inherit from JUCE's Component class for UI elements
                         public ChangeListener, // Inherit from ChangeListener to respond to changes
                         public juce::ChangeBroadcaster // Inherit from ChangeBroadcaster to notify listeners of changes
{
public:
    // Constructor declaration, initializing with an AudioFormatManager and AudioThumbnailCache
    WaveformDisplay(AudioFormatManager & formatManagerToUse, AudioThumbnailCache & cacheToUse);
    ~WaveformDisplay() override; // Destructor declaration

    void paint (juce::Graphics&) override; // Override the paint method to draw the component
    void resized() override; // Override the resized method (unused here, but required by the framework)
    
    // Override the changeListenerCallback to respond to changes in the audio thumbnail
    void changeListenerCallback (ChangeBroadcaster *source) override;
    
    // Method to load an audio file into the waveform display
    void loadFile(File audioFile);
    bool fileLoaded; // Flag to track whether a file has been successfully loaded
    
    // Set the relative position of the playback head
    void setPositionRelative(double pos);
    AudioThumbnail audioThumb; // AudioThumbnail to manage and display the waveform
private:

    double position; // Store the current playback position as a relative value
    
    // Macro to declare the class as non-copyable and to enable leak detection
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
