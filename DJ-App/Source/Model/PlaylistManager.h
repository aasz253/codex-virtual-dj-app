#pragma once
#include <JuceHeader.h>
#include "Track.h"
#include <vector>
#include <functional>

class PlaylistManager
{
public:
    PlaylistManager();
    ~PlaylistManager();
    
    // Track management
    void addTrack(const Track& track);
    void addTracks(const std::vector<Track>& tracks);
    void removeTrack(int index);
    void clearPlaylist();
    
    // File operations
    void loadTracksFromDirectory(const juce::File& directory);
    void loadTracksFromFiles(const juce::Array<juce::File>& files);
    void savePlaylist(const juce::File& file);
    void loadPlaylist(const juce::File& file);
    
    // Getters
    int getNumTracks() const { return static_cast<int>(tracks.size()); }
    const Track* getTrack(int index) const;
    Track* getTrack(int index);
    const std::vector<Track>& getAllTracks() const { return tracks; }
    
    // Search and filter
    std::vector<int> searchTracks(const juce::String& query) const;
    std::vector<int> filterByGenre(const juce::String& genre) const;
    std::vector<int> filterByArtist(const juce::String& artist) const;
    std::vector<int> filterByBPMRange(int minBPM, int maxBPM) const;
    
    // Sorting
    enum class SortCriteria
    {
        Title,
        Artist,
        Album,
        Duration,
        BPM,
        DateAdded
    };
    
    void sortTracks(SortCriteria criteria, bool ascending = true);
    
    // Playlist operations
    void shufflePlaylist();
    void moveTrack(int fromIndex, int toIndex);
    
    // Statistics
    double getTotalDuration() const;
    int getAverageBPM() const;
    juce::StringArray getUniqueGenres() const;
    juce::StringArray getUniqueArtists() const;
    
    // Callbacks
    std::function<void(int)> onTrackAdded;
    std::function<void(int)> onTrackRemoved;
    std::function<void()> onPlaylistCleared;
    std::function<void()> onPlaylistChanged;
    
private:
    std::vector<Track> tracks;
    
    // Helper methods
    bool isAudioFile(const juce::File& file) const;
    void notifyPlaylistChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistManager)
};