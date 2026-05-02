#pragma once
#include <JuceHeader.h>
#include "DeckView.h"
#include "MixerView.h"
#include "PlaylistView.h"
#include "../Controller/DJController.h"
#include "../Model/PlaylistManager.h"

class MainView : public juce::Component,
                public juce::MenuBarModel,
                public juce::ApplicationCommandTarget
{
public:
    MainView();
    ~MainView() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Menu bar
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
    
    // Application commands
    juce::ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;
    
    // Audio setup
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    
private:
    enum CommandIDs
    {
        openFile = 0x2000,
        openFolder,
        savePlaylist,
        loadPlaylist,
        clearPlaylist,
        showSettings,
        showAbout,
        toggleFullscreen,
        resetLayout
    };
    
    void setupComponents();
    void setupCallbacks();
    void setupMenuBar();
    void showSettingsDialog();
    void showAboutDialog();
    void loadAudioFiles();
    void loadAudioFolder();
    
    // Components
    std::unique_ptr<DeckView> deckView1;
    std::unique_ptr<DeckView> deckView2;
    std::unique_ptr<MixerView> mixerView;
    std::unique_ptr<PlaylistView> playlistView1;
    std::unique_ptr<PlaylistView> playlistView2;
    
    // Menu bar
    std::unique_ptr<juce::MenuBarComponent> menuBar;
    
    // Controllers and models
    std::unique_ptr<DJController> djController;
    std::unique_ptr<PlaylistManager> playlistManager1;
    std::unique_ptr<PlaylistManager> playlistManager2;
    
    // Layout settings
    bool isFullscreen = false;
    juce::Rectangle<int> windowedBounds;
    
    // Visual settings
    juce::Colour backgroundColour;
    juce::Colour panelColour;
    juce::Colour borderColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainView)
};