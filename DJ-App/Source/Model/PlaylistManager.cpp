#include "PlaylistManager.h"
#include <algorithm>
#include <random>

PlaylistManager::PlaylistManager()
{
}

PlaylistManager::~PlaylistManager()
{
}

void PlaylistManager::addTrack(const Track& track)
{
    if (track.isValid())
    {
        tracks.push_back(track);
        int index = static_cast<int>(tracks.size() - 1);
        
        if (onTrackAdded)
            onTrackAdded(index);
        
        notifyPlaylistChanged();
    }
}

void PlaylistManager::addTracks(const std::vector<Track>& newTracks)
{
    for (const auto& track : newTracks)
    {
        if (track.isValid())
        {
            tracks.push_back(track);
        }
    }
    notifyPlaylistChanged();
}

void PlaylistManager::removeTrack(int index)
{
    if (index >= 0 && index < static_cast<int>(tracks.size()))
    {
        tracks.erase(tracks.begin() + index);
        
        if (onTrackRemoved)
            onTrackRemoved(index);
        
        notifyPlaylistChanged();
    }
}

void PlaylistManager::clearPlaylist()
{
    tracks.clear();
    
    if (onPlaylistCleared)
        onPlaylistCleared();
    
    notifyPlaylistChanged();
}

void PlaylistManager::loadTracksFromDirectory(const juce::File& directory)
{
    if (!directory.isDirectory())
        return;
    
    auto files = directory.findChildFiles(juce::File::findFiles, true);
    
    for (const auto& file : files)
    {
        if (isAudioFile(file))
        {
            addTrack(Track(file));
        }
    }
}

void PlaylistManager::loadTracksFromFiles(const juce::Array<juce::File>& files)
{
    for (const auto& file : files)
    {
        if (isAudioFile(file))
        {
            addTrack(Track(file));
        }
    }
}

void PlaylistManager::savePlaylist(const juce::File& file)
{
    juce::XmlElement playlist("Playlist");
    
    for (const auto& track : tracks)
    {
        auto* trackElement = new juce::XmlElement("Track");
        trackElement->setAttribute("file", track.getFilePath());
        trackElement->setAttribute("title", track.getTitle());
        trackElement->setAttribute("artist", track.getArtist());
        trackElement->setAttribute("album", track.getAlbum());
        trackElement->setAttribute("genre", track.getGenre());
        trackElement->setAttribute("duration", track.getDuration());
        trackElement->setAttribute("bpm", track.getBPM());
        
        playlist.addChildElement(trackElement);
    }
    
    playlist.writeTo(file);
}

void PlaylistManager::loadPlaylist(const juce::File& file)
{
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr)
        return;
    
    clearPlaylist();
    
    for (auto* trackElement : xml->getChildWithTagNameIterator("Track"))
    {
        juce::File trackFile(trackElement->getStringAttribute("file"));
        if (trackFile.exists())
        {
            Track track(trackFile);
            
            // Override with saved metadata if available
            if (trackElement->hasAttribute("title"))
                track.setTitle(trackElement->getStringAttribute("title"));
            if (trackElement->hasAttribute("artist"))
                track.setArtist(trackElement->getStringAttribute("artist"));
            if (trackElement->hasAttribute("album"))
                track.setAlbum(trackElement->getStringAttribute("album"));
            if (trackElement->hasAttribute("genre"))
                track.setGenre(trackElement->getStringAttribute("genre"));
            if (trackElement->hasAttribute("bpm"))
                track.setBPM(trackElement->getIntAttribute("bpm"));
            
            addTrack(track);
        }
    }
}

const Track* PlaylistManager::getTrack(int index) const
{
    if (index >= 0 && index < static_cast<int>(tracks.size()))
        return &tracks[index];
    return nullptr;
}

Track* PlaylistManager::getTrack(int index)
{
    if (index >= 0 && index < static_cast<int>(tracks.size()))
        return &tracks[index];
    return nullptr;
}

std::vector<int> PlaylistManager::searchTracks(const juce::String& query) const
{
    std::vector<int> results;
    juce::String lowerQuery = query.toLowerCase();
    
    for (int i = 0; i < static_cast<int>(tracks.size()); ++i)
    {
        const auto& track = tracks[i];
        if (track.getTitle().toLowerCase().contains(lowerQuery) ||
            track.getArtist().toLowerCase().contains(lowerQuery) ||
            track.getAlbum().toLowerCase().contains(lowerQuery) ||
            track.getFileName().toLowerCase().contains(lowerQuery))
        {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<int> PlaylistManager::filterByGenre(const juce::String& genre) const
{
    std::vector<int> results;
    
    for (int i = 0; i < static_cast<int>(tracks.size()); ++i)
    {
        if (tracks[i].getGenre().equalsIgnoreCase(genre))
        {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<int> PlaylistManager::filterByArtist(const juce::String& artist) const
{
    std::vector<int> results;
    
    for (int i = 0; i < static_cast<int>(tracks.size()); ++i)
    {
        if (tracks[i].getArtist().equalsIgnoreCase(artist))
        {
            results.push_back(i);
        }
    }
    
    return results;
}

std::vector<int> PlaylistManager::filterByBPMRange(int minBPM, int maxBPM) const
{
    std::vector<int> results;
    
    for (int i = 0; i < static_cast<int>(tracks.size()); ++i)
    {
        int bpm = tracks[i].getBPM();
        if (bpm >= minBPM && bpm <= maxBPM)
        {
            results.push_back(i);
        }
    }
    
    return results;
}

void PlaylistManager::sortTracks(SortCriteria criteria, bool ascending)
{
    std::sort(tracks.begin(), tracks.end(), [criteria, ascending](const Track& a, const Track& b)
    {
        bool result = false;
        
        switch (criteria)
        {
            case SortCriteria::Title:
                result = a.getTitle().compareIgnoreCase(b.getTitle()) < 0;
                break;
            case SortCriteria::Artist:
                result = a.getArtist().compareIgnoreCase(b.getArtist()) < 0;
                break;
            case SortCriteria::Album:
                result = a.getAlbum().compareIgnoreCase(b.getAlbum()) < 0;
                break;
            case SortCriteria::Duration:
                result = a.getDuration() < b.getDuration();
                break;
            case SortCriteria::BPM:
                result = a.getBPM() < b.getBPM();
                break;
            case SortCriteria::DateAdded:
                // For now, maintain current order for date added
                result = false;
                break;
        }
        
        return ascending ? result : !result;
    });
    
    notifyPlaylistChanged();
}

void PlaylistManager::shufflePlaylist()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tracks.begin(), tracks.end(), g);
    notifyPlaylistChanged();
}

void PlaylistManager::moveTrack(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < static_cast<int>(tracks.size()) &&
        toIndex >= 0 && toIndex < static_cast<int>(tracks.size()) &&
        fromIndex != toIndex)
    {
        Track track = tracks[fromIndex];
        tracks.erase(tracks.begin() + fromIndex);
        tracks.insert(tracks.begin() + toIndex, track);
        notifyPlaylistChanged();
    }
}

double PlaylistManager::getTotalDuration() const
{
    double total = 0.0;
    for (const auto& track : tracks)
    {
        total += track.getDuration();
    }
    return total;
}

int PlaylistManager::getAverageBPM() const
{
    if (tracks.empty())
        return 0;
    
    int total = 0;
    int count = 0;
    
    for (const auto& track : tracks)
    {
        if (track.getBPM() > 0)
        {
            total += track.getBPM();
            count++;
        }
    }
    
    return count > 0 ? total / count : 0;
}

juce::StringArray PlaylistManager::getUniqueGenres() const
{
    juce::StringArray genres;
    
    for (const auto& track : tracks)
    {
        if (!genres.contains(track.getGenre(), true))
        {
            genres.add(track.getGenre());
        }
    }
    
    return genres;
}

juce::StringArray PlaylistManager::getUniqueArtists() const
{
    juce::StringArray artists;
    
    for (const auto& track : tracks)
    {
        if (!artists.contains(track.getArtist(), true))
        {
            artists.add(track.getArtist());
        }
    }
    
    return artists;
}

bool PlaylistManager::isAudioFile(const juce::File& file) const
{
    return file.hasFileExtension(".mp3;.wav;.flac;.aac;.m4a;.ogg;.wma");
}

void PlaylistManager::notifyPlaylistChanged()
{
    if (onPlaylistChanged)
        onPlaylistChanged();
}