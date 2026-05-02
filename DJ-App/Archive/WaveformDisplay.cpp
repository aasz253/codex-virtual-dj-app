#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager & formatManagerToUse, AudioThumbnailCache & cacheToUse)
                            : fileLoaded(false),// Initialize fileLoaded to false
                              audioThumb(1000, formatManagerToUse, cacheToUse), // Initialize the AudioThumbnail
                              position(0) // Initialize the position to 0
{
    // Add this WaveformDisplay as a listener to the audioThumb to receive change notifications
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (juce::Colours::orangered);
    if(fileLoaded)
    {
        //Draw thumbNail
        audioThumb.drawChannel(g, getLocalBounds(), 0, audioThumb.getTotalLength(), 0, 1.0f);
        // Draw a line representing the current playback position
        g.setColour(Colours::red);
        g.drawRect(position * getWidth(), 0, getWidth()/30, getHeight());
    }
    else // If no file has been loaded
    {
        // Display a placeholder text
        g.setFont (18.0f);
        g.drawText ("Wave not loaded...", getLocalBounds(),
        juce::Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
}

void WaveformDisplay::loadFile(File audioFile)
{
    /**Clear any previous audioThumb**/
    audioThumb.clear();
    /**So, get file ... FileInputSource() unpack the file and use setSource() "good to go"**/
    fileLoaded = audioThumb.setSource(new juce::FileInputSource(audioFile));
}


void WaveformDisplay::changeListenerCallback (ChangeBroadcaster *source)
{
    repaint(); // Repaint the component to update the waveform display
}

void WaveformDisplay::setPositionRelative(double pos)
{
    // Set the playback position marker to a new position and repaint if it has changed
    if(pos != position) // Check if the new position is different from the current position
    {
        position = pos; // Update the position
        repaint(); // Repaint the component to reflect the new position
    }
}
