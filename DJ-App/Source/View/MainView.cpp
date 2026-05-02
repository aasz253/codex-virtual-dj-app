#include "MainView.h"

MainView::MainView()
    : backgroundColour(juce::Colour(0xff0f0f0f)),
      panelColour(juce::Colour(0xff1a1a1a)),
      borderColour(juce::Colour(0xff404040))
{
    setupComponents();
    setupCallbacks();
    setupMenuBar();
    
    setSize(1400, 900);
    setWantsKeyboardFocus(true);
}

MainView::~MainView() = default;

void MainView::setupComponents()
{
    // Create controllers and models
    djController = std::make_unique<DJController>();
    playlistManager1 = std::make_unique<PlaylistManager>();
    playlistManager2 = std::make_unique<PlaylistManager>();
    
    // Create deck views
    deckView1 = std::make_unique<DeckView>(*djController, 1);
    deckView2 = std::make_unique<DeckView>(*djController, 2);
    addAndMakeVisible(*deckView1);
    addAndMakeVisible(*deckView2);
    
    // Create mixer view
    mixerView = std::make_unique<MixerView>();
    addAndMakeVisible(*mixerView);
    
    // Create playlist views
    playlistView1 = std::make_unique<PlaylistView>();
    playlistView2 = std::make_unique<PlaylistView>();
    playlistView1->setPlaylistManager(playlistManager1.get());
    playlistView2->setPlaylistManager(playlistManager2.get());
    addAndMakeVisible(*playlistView1);
    addAndMakeVisible(*playlistView2);
    
    // Setup menu bar
    menuBar = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(*menuBar);
}

void MainView::setupCallbacks()
{
    // Note: DeckView callbacks are handled internally through button listeners
    

    
    // Mixer callbacks
    mixerView->onCrossfaderChanged = [this](float value) {
        djController->setCrossfader(value);
    };
    
    mixerView->onMasterGainChanged = [this](float gain) {
        djController->setMasterGain(gain);
    };
    
    mixerView->onCueGainChanged = [this](float gain) {
        djController->setCueGain(gain);
    };
    
    mixerView->onRecordingToggled = [this](bool recording) {
        if (recording)
            djController->startRecording(juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("DJ_Recording.wav"));
        else
            djController->stopRecording();
    };
    
    mixerView->onBeatSyncPressed = [this]() {
        djController->enableBeatSync(true);
    };
    
    // Playlist 1 callbacks
    playlistView1->onTrackLoadRequested = [this](int trackIndex, int deckNumber) {
        auto track = playlistManager1->getTrack(trackIndex);
        if (track)
        {
            // Convert 1-based deck number to 0-based index
            int deckIndex = deckNumber - 1;
            djController->loadTrackToDeck(deckIndex, *track);
        }
    };
    
    // Playlist 2 callbacks
    playlistView2->onTrackLoadRequested = [this](int trackIndex, int deckNumber) {
        auto track = playlistManager2->getTrack(trackIndex);
        if (track)
        {
            // Convert 1-based deck number to 0-based index
            int deckIndex = deckNumber - 1;
            djController->loadTrackToDeck(deckIndex, *track);
        }
    };
    
    // DJ Controller callbacks
    djController->onDeckPositionChanged = [this](int deckId, double position) {
        if (deckId == 1)
            deckView1->updatePosition(position);
        else if (deckId == 2)
            deckView2->updatePosition(position);
    };
    
    djController->onDeckLevelsChanged = [this](int deckId, float rms, float peak) {
        if (deckId == 1)
            deckView1->updateLevels(rms, peak);
        else if (deckId == 2)
            deckView2->updateLevels(rms, peak);
    };
}

void MainView::setupMenuBar()
{
    menuBar->setModel(this);
}

void MainView::paint(juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient gradient(backgroundColour.brighter(0.1f), 0, 0,
                                 backgroundColour.darker(0.2f), 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Panel separators
    g.setColour(borderColour);
    
    auto bounds = getLocalBounds();
    bounds.removeFromTop(25); // Menu bar space
    
    int deckWidth = (bounds.getWidth() - 200) / 2; // 200px for mixer
    int playlistHeight = bounds.getHeight() / 2;
    
    // Vertical separators
    g.drawVerticalLine(deckWidth, bounds.getY(), bounds.getBottom());
    g.drawVerticalLine(deckWidth + 200, bounds.getY(), bounds.getBottom());
    
    // Horizontal separators
    g.drawHorizontalLine(bounds.getY() + playlistHeight, 0, bounds.getWidth());
}

void MainView::resized()
{
    auto bounds = getLocalBounds();
    
    // Menu bar
    menuBar->setBounds(bounds.removeFromTop(25));
    
    // Main layout
    int deckWidth = (bounds.getWidth() - 200) / 2; // 200px for mixer
    int playlistHeight = bounds.getHeight() / 2;
    
    // Top row - Decks and mixer
    auto topRow = bounds.removeFromTop(playlistHeight);
    deckView1->setBounds(topRow.removeFromLeft(deckWidth));
    mixerView->setBounds(topRow.removeFromLeft(200));
    deckView2->setBounds(topRow);
    
    // Bottom row - Playlists
    auto bottomRow = bounds;
    int playlistWidth = bottomRow.getWidth() / 2;
    playlistView1->setBounds(bottomRow.removeFromLeft(playlistWidth));
    playlistView2->setBounds(bottomRow);
}

// Menu bar implementation
juce::StringArray MainView::getMenuBarNames()
{
    return {"File", "View", "Help"};
}

juce::PopupMenu MainView::getMenuForIndex(int topLevelMenuIndex, const juce::String& menuName)
{
    juce::PopupMenu menu;
    
    if (topLevelMenuIndex == 0) // File
    {
        menu.addItem(openFile, "Open Audio File...", true, false);
        menu.addItem(openFolder, "Open Folder...", true, false);
        menu.addSeparator();
        menu.addItem(savePlaylist, "Save Playlist...", true, false);
        menu.addItem(loadPlaylist, "Load Playlist...", true, false);
        menu.addItem(clearPlaylist, "Clear Playlist", true, false);
        menu.addSeparator();
        menu.addItem(showSettings, "Settings...", true, false);
    }
    else if (topLevelMenuIndex == 1) // View
    {
        menu.addItem(toggleFullscreen, "Toggle Fullscreen", true, isFullscreen);
        menu.addItem(resetLayout, "Reset Layout", true, false);
    }
    else if (topLevelMenuIndex == 2) // Help
    {
        menu.addItem(showAbout, "About DJ App...", true, false);
    }
    
    return menu;
}

void MainView::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
    switch (menuItemID)
    {
        case openFile:
            loadAudioFiles();
            break;
        case openFolder:
            loadAudioFolder();
            break;
        case savePlaylist:
            // Implement save playlist
            break;
        case loadPlaylist:
            // Implement load playlist
            break;
        case clearPlaylist:
            playlistManager1->clearPlaylist();
            playlistManager2->clearPlaylist();
            playlistView1->refreshPlaylist();
            playlistView2->refreshPlaylist();
            break;
        case showSettings:
            showSettingsDialog();
            break;
        case showAbout:
            showAboutDialog();
            break;
        case toggleFullscreen:
            // Implement fullscreen toggle
            break;
        case resetLayout:
            resized();
            break;
    }
}

// Application commands
juce::ApplicationCommandTarget* MainView::getNextCommandTarget()
{
    return nullptr;
}

void MainView::getAllCommands(juce::Array<juce::CommandID>& commands)
{
    commands.add(openFile);
    commands.add(openFolder);
    commands.add(savePlaylist);
    commands.add(loadPlaylist);
    commands.add(clearPlaylist);
    commands.add(showSettings);
    commands.add(showAbout);
    commands.add(toggleFullscreen);
    commands.add(resetLayout);
}

void MainView::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
        case openFile:
            result.setInfo("Open File", "Open audio file", "File", 0);
            result.addDefaultKeypress('o', juce::ModifierKeys::commandModifier);
            break;
        case openFolder:
            result.setInfo("Open Folder", "Open folder", "File", 0);
            result.addDefaultKeypress('o', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier);
            break;
        case toggleFullscreen:
            result.setInfo("Toggle Fullscreen", "Toggle fullscreen mode", "View", 0);
            result.addDefaultKeypress(juce::KeyPress::F11Key, 0);
            break;
    }
}

bool MainView::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
        case openFile:
            loadAudioFiles();
            return true;
        case openFolder:
            loadAudioFolder();
            return true;
        case toggleFullscreen:
            // Implement fullscreen toggle
            return true;
        default:
            return false;
    }
}

// Audio methods
void MainView::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    djController->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainView::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    djController->getNextAudioBlock(bufferToFill);
}

void MainView::releaseResources()
{
    djController->releaseResources();
}

// Helper methods
void MainView::loadAudioFiles()
{
    auto chooser = std::make_unique<juce::FileChooser>("Select audio files", juce::File(), "*.mp3;*.wav;*.flac;*.aac;*.ogg");
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
                         [this](const juce::FileChooser& fc)
                         {
                             auto files = fc.getResults();
                             if (!files.isEmpty() && playlistManager1)
                             {
                                 playlistManager1->loadTracksFromFiles(files);
                                 playlistView1->refreshPlaylist();
                             }
                         });
}

void MainView::loadAudioFolder()
{
    auto chooser = std::make_unique<juce::FileChooser>("Select folder", juce::File());
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                         [this](const juce::FileChooser& fc)
                         {
                             auto folder = fc.getResult();
                             if (folder != juce::File{} && playlistManager1)
                             {
                                 playlistManager1->loadTracksFromDirectory(folder);
                                 playlistView1->refreshPlaylist();
                             }
                         });
}

void MainView::showSettingsDialog()
{
    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "Settings",
        "Settings dialog not yet implemented.",
        "OK"
    );
}

void MainView::showAboutDialog()
{
    juce::AlertWindow::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "About DJ App",
        "Professional DJ Desktop Application\n\nBuilt with JUCE Framework\nVersion 1.0",
        "OK"
    );
}