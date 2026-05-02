#include "PlaylistView.h"

PlaylistView::PlaylistView()
    : backgroundColour(juce::Colour(0xff1a1a1a)),
      alternateRowColour(juce::Colour(0xff252525)),
      selectedRowColour(juce::Colour(0xff0066cc)),
      textColour(juce::Colour(0xffeeeeee)),
      headerColour(juce::Colour(0xff333333))
{
    setupComponents();
    setupTableHeader();
    startTimerHz(10); // 10 FPS for stats updates
}

PlaylistView::~PlaylistView()
{
    stopTimer();
}

void PlaylistView::setupComponents()
{
    // Title label
    titleLabel = std::make_unique<juce::Label>("title", "PLAYLIST");
    titleLabel->setFont(juce::Font(16.0f, juce::Font::bold));
    titleLabel->setColour(juce::Label::textColourId, textColour);
    titleLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*titleLabel);
    
    // Search box
    searchBox = std::make_unique<juce::TextEditor>("search");
    searchBox->setTextToShowWhenEmpty("Search tracks...", juce::Colour(0xff666666));
    searchBox->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff2a2a2a));
    searchBox->setColour(juce::TextEditor::textColourId, textColour);
    searchBox->setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff404040));
    searchBox->addListener(this);
    addAndMakeVisible(*searchBox);
    
    // Table
    tableListBox = std::make_unique<juce::TableListBox>("playlist", this);
    tableListBox->setColour(juce::ListBox::backgroundColourId, backgroundColour);
    tableListBox->setColour(juce::ListBox::outlineColourId, juce::Colour(0xff404040));
    tableListBox->setOutlineThickness(1);
    tableListBox->setMultipleSelectionEnabled(false);
    addAndMakeVisible(*tableListBox);
    
    // Control buttons
    addFilesButton = std::make_unique<ModernButton>("Add Files");
    addFilesButton->setButtonStyle(ModernButton::Style::Primary);
    addFilesButton->addListener(this);
    addAndMakeVisible(*addFilesButton);
    
    addFolderButton = std::make_unique<ModernButton>("Add Folder");
    addFolderButton->setButtonStyle(ModernButton::Style::Secondary);
    addFolderButton->addListener(this);
    addAndMakeVisible(*addFolderButton);
    
    removeButton = std::make_unique<ModernButton>("Remove");
    removeButton->setButtonStyle(ModernButton::Style::Danger);
    removeButton->addListener(this);
    addAndMakeVisible(*removeButton);
    
    clearButton = std::make_unique<ModernButton>("Clear All");
    clearButton->setButtonStyle(ModernButton::Style::Danger);
    clearButton->addListener(this);
    addAndMakeVisible(*clearButton);
    
    shuffleButton = std::make_unique<ModernButton>("Shuffle");
    shuffleButton->setButtonStyle(ModernButton::Style::Secondary);
    shuffleButton->addListener(this);
    addAndMakeVisible(*shuffleButton);
    
    savePlaylistButton = std::make_unique<ModernButton>("Save");
    savePlaylistButton->setButtonStyle(ModernButton::Style::Secondary);
    savePlaylistButton->addListener(this);
    addAndMakeVisible(*savePlaylistButton);
    
    loadPlaylistButton = std::make_unique<ModernButton>("Load");
    loadPlaylistButton->setButtonStyle(ModernButton::Style::Secondary);
    loadPlaylistButton->addListener(this);
    addAndMakeVisible(*loadPlaylistButton);
    
    loadMusicFolderButton = std::make_unique<ModernButton>("Load Music");
    loadMusicFolderButton->setButtonStyle(ModernButton::Style::Primary);
    loadMusicFolderButton->addListener(this);
    addAndMakeVisible(*loadMusicFolderButton);
    
    // Stats label
    statsLabel = std::make_unique<juce::Label>("stats", "0 tracks");
    statsLabel->setFont(juce::Font(12.0f));
    statsLabel->setColour(juce::Label::textColourId, juce::Colour(0xffaaaaaa));
    statsLabel->setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(*statsLabel);
}

void PlaylistView::setupTableHeader()
{
    auto& header = tableListBox->getHeader();
    
    header.addColumn("#", 1, 40, 30, 50, juce::TableHeaderComponent::notResizable);
    header.addColumn("Title", 2, 200, 100, 400, juce::TableHeaderComponent::defaultFlags);
    header.addColumn("Artist", 3, 150, 80, 300, juce::TableHeaderComponent::defaultFlags);
    header.addColumn("Album", 4, 150, 80, 300, juce::TableHeaderComponent::defaultFlags);
    header.addColumn("Genre", 5, 100, 60, 200, juce::TableHeaderComponent::defaultFlags);
    header.addColumn("Duration", 6, 80, 60, 100, juce::TableHeaderComponent::defaultFlags);
    header.addColumn("BPM", 7, 60, 50, 80, juce::TableHeaderComponent::defaultFlags);
    header.addColumn("Load", 8, 80, 80, 80, juce::TableHeaderComponent::notResizable);
    
    header.setColour(juce::TableHeaderComponent::backgroundColourId, headerColour);
    header.setColour(juce::TableHeaderComponent::textColourId, textColour);
    header.setColour(juce::TableHeaderComponent::outlineColourId, juce::Colour(0xff404040));
    
    header.setSortColumnId(sortColumnId, sortAscending);
}

void PlaylistView::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
    
    // Border
    g.setColour(juce::Colour(0xff404040));
    g.drawRect(getLocalBounds(), 1);
}

void PlaylistView::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    
    // Title
    titleLabel->setBounds(bounds.removeFromTop(25));
    bounds.removeFromTop(5);
    
    // Search box
    searchBox->setBounds(bounds.removeFromTop(25).reduced(5, 0));
    bounds.removeFromTop(5);
    
    // Control buttons (top row)
    auto buttonArea1 = bounds.removeFromTop(30);
    int buttonWidth = buttonArea1.getWidth() / 5;
    addFilesButton->setBounds(buttonArea1.removeFromLeft(buttonWidth).reduced(2));
    addFolderButton->setBounds(buttonArea1.removeFromLeft(buttonWidth).reduced(2));
    loadMusicFolderButton->setBounds(buttonArea1.removeFromLeft(buttonWidth).reduced(2));
    removeButton->setBounds(buttonArea1.removeFromLeft(buttonWidth).reduced(2));
    clearButton->setBounds(buttonArea1.reduced(2));
    
    bounds.removeFromTop(5);
    
    // Control buttons (bottom row)
    auto buttonArea2 = bounds.removeFromTop(30);
    buttonWidth = buttonArea2.getWidth() / 3;
    shuffleButton->setBounds(buttonArea2.removeFromLeft(buttonWidth).reduced(2));
    savePlaylistButton->setBounds(buttonArea2.removeFromLeft(buttonWidth).reduced(2));
    loadPlaylistButton->setBounds(buttonArea2.reduced(2));
    
    bounds.removeFromTop(5);
    
    // Stats label
    statsLabel->setBounds(bounds.removeFromBottom(20));
    bounds.removeFromBottom(5);
    
    // Table
    tableListBox->setBounds(bounds);
}

int PlaylistView::getNumRows()
{
    return static_cast<int>(filteredTrackIndices.size());
}

void PlaylistView::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.setColour(selectedRowColour);
        g.fillAll();
    }
    else if (rowNumber % 2 == 1)
    {
        g.setColour(alternateRowColour);
        g.fillAll();
    }
}

void PlaylistView::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (!playlistManager || rowNumber >= static_cast<int>(filteredTrackIndices.size()))
        return;
    
    int trackIndex = filteredTrackIndices[rowNumber];
    auto track = playlistManager->getTrack(trackIndex);
    
    if (!track)
        return;
    
    g.setColour(rowIsSelected ? juce::Colour(0xffffffff) : textColour);
    g.setFont(juce::Font(12.0f));
    
    juce::String text;
    
    switch (columnId)
    {
        case 1: // Track number
            text = juce::String(rowNumber + 1);
            break;
        case 2: // Title
            text = track->getTitle();
            break;
        case 3: // Artist
            text = track->getArtist();
            break;
        case 4: // Album
            text = track->getAlbum();
            break;
        case 5: // Genre
            text = track->getGenre();
            break;
        case 6: // Duration
            text = formatDuration(track->getDuration());
            break;
        case 7: // BPM
            if (track->getBPM() > 0.0f)
                text = juce::String(static_cast<float>(track->getBPM()), 1, false);
            break;
        case 8: // Load column - handled by component
            return;
    }
    
    g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
}

juce::Component* PlaylistView::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    if (columnId == 8) // Load column
    {
        if (rowNumber < static_cast<int>(filteredTrackIndices.size()))
        {
            int trackIndex = filteredTrackIndices[rowNumber];
            
            auto* deckComponent = dynamic_cast<DeckSelectionComponent*>(existingComponentToUpdate);
            if (deckComponent == nullptr)
                deckComponent = new DeckSelectionComponent(*this, trackIndex);
            else
                deckComponent->setTrackIndex(trackIndex);
            
            return deckComponent;
        }
    }
    
    return nullptr;
}

void PlaylistView::cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
    if (rowNumber >= 0 && rowNumber < static_cast<int>(filteredTrackIndices.size()))
    {
        selectedRow = rowNumber;
        tableListBox->selectRow(rowNumber);
        
        if (onTrackSelected)
            onTrackSelected(filteredTrackIndices[rowNumber]);
    }
}

void PlaylistView::cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event)
{
    if (rowNumber >= 0 && rowNumber < static_cast<int>(filteredTrackIndices.size()) && onTrackLoadRequested)
    {
        // Double-click loads to deck 1 by default
        onTrackLoadRequested(filteredTrackIndices[rowNumber], 1);
    }
}

void PlaylistView::buttonClicked(juce::Button* button)
{
    if (button == addFilesButton.get())
    {
        auto chooser = std::make_unique<juce::FileChooser>("Select audio files", juce::File(), "*.mp3;*.wav;*.flac;*.aac;*.ogg");
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
                             [this](const juce::FileChooser& fc)
                             {
                                 auto files = fc.getResults();
                                 if (!files.isEmpty() && playlistManager)
                                 {
                                     playlistManager->loadTracksFromFiles(files);
                                     refreshPlaylist();
                                 }
                             });
    }
    else if (button == addFolderButton.get())
    {
        auto chooser = std::make_unique<juce::FileChooser>("Select folder", juce::File());
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
                             [this](const juce::FileChooser& fc)
                             {
                                 auto folder = fc.getResult();
                                 if (folder != juce::File{} && playlistManager)
                                 {
                                     playlistManager->loadTracksFromDirectory(folder);
                                     refreshPlaylist();
                                 }
                             });
    }
    else if (button == removeButton.get())
    {
        if (selectedRow >= 0 && selectedRow < static_cast<int>(filteredTrackIndices.size()) && playlistManager)
        {
            int trackIndex = filteredTrackIndices[selectedRow];
            playlistManager->removeTrack(trackIndex);
            refreshPlaylist();
        }
    }
    else if (button == clearButton.get())
    {
        if (playlistManager)
        {
            playlistManager->clearPlaylist();
            refreshPlaylist();
        }
    }
    else if (button == shuffleButton.get())
    {
        if (playlistManager)
        {
            playlistManager->shufflePlaylist();
            refreshPlaylist();
        }
    }
    else if (button == savePlaylistButton.get())
    {
        auto chooser = std::make_unique<juce::FileChooser>("Save playlist", juce::File(), "*.m3u");
        chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                            [this](const juce::FileChooser& fc)
                            {
                                auto file = fc.getResult();
                                if (file != juce::File{} && playlistManager)
                                    playlistManager->savePlaylist(file);
                            });
    }
    else if (button == loadPlaylistButton.get())
    {
        auto chooser = std::make_unique<juce::FileChooser>("Load playlist", juce::File(), "*.m3u");
        chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                             [this](const juce::FileChooser& fc)
                             {
                                 auto file = fc.getResult();
                                 if (file != juce::File{} && playlistManager)
                                 {
                                     playlistManager->loadPlaylist(file);
                                     refreshPlaylist();
                                 }
                             });
    }
    else if (button == loadMusicFolderButton.get())
    {
        // Directly load from ~/Music folder
        juce::File musicFolder = juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile("Music");
        if (musicFolder.exists() && playlistManager)
        {
            playlistManager->loadTracksFromDirectory(musicFolder);
            refreshPlaylist();
        }
    }
}

void PlaylistView::textEditorTextChanged(juce::TextEditor& editor)
{
    if (&editor == searchBox.get())
    {
        setSearchFilter(editor.getText());
    }
}

void PlaylistView::timerCallback()
{
    // Update stats
    if (playlistManager)
    {
        int totalTracks = playlistManager->getNumTracks();
        int filteredTracks = static_cast<int>(filteredTrackIndices.size());
        double totalDuration = playlistManager->getTotalDuration();
        
        juce::String statsText = juce::String(filteredTracks);
        if (filteredTracks != totalTracks)
            statsText += " of " + juce::String(totalTracks);
        statsText += " tracks";
        
        if (totalDuration > 0.0)
            statsText += " • " + formatDuration(totalDuration);
        
        statsLabel->setText(statsText, juce::dontSendNotification);
    }
}

void PlaylistView::setPlaylistManager(PlaylistManager* manager)
{
    playlistManager = manager;
    refreshPlaylist();
}

void PlaylistView::refreshPlaylist()
{
    updateFilteredTracks();
    tableListBox->updateContent();
    
    if (onPlaylistChanged)
        onPlaylistChanged();
}

void PlaylistView::selectTrack(int index)
{
    // Find the row number for this track index
    for (int i = 0; i < static_cast<int>(filteredTrackIndices.size()); ++i)
    {
        if (filteredTrackIndices[i] == index)
        {
            selectedRow = i;
            tableListBox->selectRow(i);
            tableListBox->scrollToEnsureRowIsOnscreen(i);
            break;
        }
    }
}

void PlaylistView::setSearchFilter(const juce::String& filter)
{
    currentSearchFilter = filter;
    updateFilteredTracks();
    tableListBox->updateContent();
}

void PlaylistView::setSortColumn(int columnId, bool ascending)
{
    sortColumnId = columnId;
    sortAscending = ascending;
    updateFilteredTracks();
    tableListBox->updateContent();
}

void PlaylistView::updateFilteredTracks()
{
    filteredTrackIndices.clear();
    
    if (!playlistManager)
        return;
    
    // Apply search filter
    if (currentSearchFilter.isEmpty())
    {
        // No filter - include all tracks
        for (int i = 0; i < playlistManager->getNumTracks(); ++i)
            filteredTrackIndices.push_back(i);
    }
    else
    {
        // Apply search filter
        auto searchResults = playlistManager->searchTracks(currentSearchFilter);
        filteredTrackIndices = searchResults;
    }
    
    // Apply sorting
    std::sort(filteredTrackIndices.begin(), filteredTrackIndices.end(),
              [this](int a, int b) {
                  auto trackA = playlistManager->getTrack(a);
                  auto trackB = playlistManager->getTrack(b);
                  
                  if (!trackA || !trackB)
                      return false;
                  
                  bool result = false;
                  
                  switch (sortColumnId)
                  {
                      case 2: // Title
                          result = trackA->getTitle().compareIgnoreCase(trackB->getTitle()) < 0;
                          break;
                      case 3: // Artist
                          result = trackA->getArtist().compareIgnoreCase(trackB->getArtist()) < 0;
                          break;
                      case 4: // Album
                          result = trackA->getAlbum().compareIgnoreCase(trackB->getAlbum()) < 0;
                          break;
                      case 5: // Genre
                          result = trackA->getGenre().compareIgnoreCase(trackB->getGenre()) < 0;
                          break;
                      case 6: // Duration
                          result = trackA->getDuration() < trackB->getDuration();
                          break;
                      case 7: // BPM
                          result = trackA->getBPM() < trackB->getBPM();
                          break;
                      default: // Track number
                          result = a < b;
                          break;
                  }
                  
                  return sortAscending ? result : !result;
              });
}

juce::String PlaylistView::formatDuration(double seconds) const
{
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return juce::String::formatted("%d:%02d", minutes, secs);
}

// DeckSelectionComponent implementation
DeckSelectionComponent::DeckSelectionComponent(PlaylistView& owner, int trackIndex)
    : owner(owner), trackIndex(trackIndex)
{
    deck1Button = std::make_unique<ModernButton>("1");
    deck1Button->setButtonStyle(ModernButton::Style::Primary);
    deck1Button->addListener(this);
    addAndMakeVisible(*deck1Button);
    
    deck2Button = std::make_unique<ModernButton>("2");
    deck2Button->setButtonStyle(ModernButton::Style::Secondary);
    deck2Button->addListener(this);
    addAndMakeVisible(*deck2Button);
}

DeckSelectionComponent::~DeckSelectionComponent() = default;

void DeckSelectionComponent::resized()
{
    auto bounds = getLocalBounds().reduced(2);
    int buttonWidth = bounds.getWidth() / 2;
    
    deck1Button->setBounds(bounds.removeFromLeft(buttonWidth).reduced(1));
    deck2Button->setBounds(bounds.reduced(1));
}

void DeckSelectionComponent::buttonClicked(juce::Button* button)
{
    int deckNumber = (button == deck1Button.get()) ? 1 : 2;
    
    if (owner.onTrackLoadRequested)
        owner.onTrackLoadRequested(trackIndex, deckNumber);
}