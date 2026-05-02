#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "PlaylistComponent.h"
#include "VerticalGradientMeter.h"
#include "DJDisk.h"

class DeckGUI  : public juce::Component,
                 public Button::Listener,
                 public Slider::Listener,
                 public Timer
{
public:
    DeckGUI(DJAudioPlayer* player,
            AudioFormatManager & formatManagerToUse,
            AudioThumbnailCache & cacheToUse);
    ~DeckGUI() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    // Implement Button::Listener
    void buttonClicked (Button *) override;
    // Implement Slider::Listener
    void sliderValueChanged (Slider *slider) override;

    void timerCallback() override;
    
    void loadFileAndDisplay(const juce::File& file);
    
    void updateDiskRotation();
    
private:
    
//    TextButton loadButton{"PRESS TO LOAD OR DROP A FILE"};
    Slider volSlider;
    Slider speedSlider;
    
    DJAudioPlayer* player;
    juce::FileChooser fChooser{"Select the file(s)..."};
    
    PlaylistComponent playlistComponent;
    
    Gui::VerticalGradientMeter meter;
    
    DJDisk disk;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
