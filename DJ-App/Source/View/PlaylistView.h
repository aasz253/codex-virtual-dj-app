#pragma once
#include <JuceHeader.h>
#include "../Model/PlaylistManager.h"
#include "../Components/ModernButton.h"

class PlaylistView : public juce::Component,
                    public juce::TableListBoxModel,
                    public juce::Button::Listener,
                    public juce::TextEditor::Listener,
                    public juce::Timer
{
public:
    PlaylistView();
    ~PlaylistView() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // TableListBoxModel
    int getNumRows() override;
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    void cellClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;
    
    // Button listener
    void buttonClicked(juce::Button* button) override;
    
    // Text editor listener
    void textEditorTextChanged(juce::TextEditor& editor) override;
    
    // Timer
    void timerCallback() override;
    
    // Playlist management
    void setPlaylistManager(PlaylistManager* manager);
    void refreshPlaylist();
    void selectTrack(int index);
    
    // Search and filter
    void setSearchFilter(const juce::String& filter);
    void setSortColumn(int columnId, bool ascending);
    
    // Callbacks
    std::function<void(int, int)> onTrackLoadRequested; // trackIndex, deckNumber
    std::function<void(int)> onTrackSelected;
    std::function<void()> onPlaylistChanged;
    
private:
    void setupComponents();
    void setupTableHeader();
    void updateFilteredTracks();
    juce::String formatDuration(double seconds) const;
    
    // Components
    std::unique_ptr<juce::TableListBox> tableListBox;
    std::unique_ptr<juce::TextEditor> searchBox;
    std::unique_ptr<ModernButton> loadButton;
    std::unique_ptr<ModernButton> removeButton;
    std::unique_ptr<ModernButton> clearButton;
    std::unique_ptr<ModernButton> shuffleButton;
    std::unique_ptr<ModernButton> addFilesButton;
    std::unique_ptr<ModernButton> addFolderButton;
    std::unique_ptr<ModernButton> savePlaylistButton;
    std::unique_ptr<ModernButton> loadPlaylistButton;
    std::unique_ptr<ModernButton> loadMusicFolderButton;
    
    // Labels
    std::unique_ptr<juce::Label> titleLabel;
    std::unique_ptr<juce::Label> statsLabel;
    
    // Data
    PlaylistManager* playlistManager = nullptr;
    std::vector<int> filteredTrackIndices;
    juce::String currentSearchFilter;
    int selectedRow = -1;
    int sortColumnId = 1; // Default sort by title
    bool sortAscending = true;
    
    // Visual settings
    juce::Colour backgroundColour;
    juce::Colour alternateRowColour;
    juce::Colour selectedRowColour;
    juce::Colour textColour;
    juce::Colour headerColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistView)
};

// Custom component for deck selection buttons in table cells
class DeckSelectionComponent : public juce::Component,
                              public juce::Button::Listener
{
public:
    DeckSelectionComponent(PlaylistView& owner, int trackIndex);
    ~DeckSelectionComponent() override;
    
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    
    int getTrackIndex() const { return trackIndex; }
    void setTrackIndex(int newTrackIndex) { trackIndex = newTrackIndex; }
    
private:
    PlaylistView& owner;
    int trackIndex;
    std::unique_ptr<ModernButton> deck1Button;
    std::unique_ptr<ModernButton> deck2Button;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckSelectionComponent)
};