#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"
#include "WaveformDisplay.h"

//MainComponent is subclass of AudioAppComponent
class MainComponent  : public juce::AudioAppComponent
{
public:

    MainComponent(AudioFormatManager & formatManagerToUse, AudioThumbnailCache & cacheToUse);

    ~MainComponent() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    //equivalent to draw() in p5. It runs until app is quit
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    //called when app exits, used to free up audio resources
    void releaseResources() override;

    void paint (juce::Graphics& g) override;
    
    void resized() override;

private:

    AudioFormatManager formatManager;
    AudioThumbnail audioThumb;
    //load up to 100 caches
    class AudioThumbnailCache thumbCache{100};
    
    DJAudioPlayer player1{formatManager, thumbCache};
    DeckGUI deckGUI1{&player1, formatManager, thumbCache};
    
    DJAudioPlayer player2{formatManager, thumbCache};
    DeckGUI deckGUI2{&player2, formatManager, thumbCache};
    
    MixerAudioSource mixerSource;
    
    PlaylistComponent playlistComponent1{&player1, formatManager, thumbCache};
    PlaylistComponent playlistComponent2{&player2, formatManager, thumbCache};
    
    juce::FileChooser fChooser{"Select the file(s)..."};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
