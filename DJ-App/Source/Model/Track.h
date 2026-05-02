#pragma once
#include <JuceHeader.h>

class Track
{
public:
    Track() = default;
    Track(const juce::File& file);
    
    // Getters
    const juce::String& getTitle() const { return title; }
    const juce::String& getArtist() const { return artist; }
    const juce::String& getAlbum() const { return album; }
    const juce::String& getGenre() const { return genre; }
    double getDuration() const { return duration; }
    int getBPM() const { return bpm; }
    const juce::File& getFile() const { return file; }
    juce::String getFilePath() const { return file.getFullPathName(); }
    juce::String getFileName() const { return file.getFileNameWithoutExtension(); }	
    
    // Setters
    void setTitle(const juce::String& newTitle) { title = newTitle; }
    void setArtist(const juce::String& newArtist) { artist = newArtist; }
    void setAlbum(const juce::String& newAlbum) { album = newAlbum; }
    void setGenre(const juce::String& newGenre) { genre = newGenre; }
    void setDuration(double newDuration) { duration = newDuration; }
    void setBPM(int newBPM) { bpm = newBPM; }
    
    // Utility methods
    bool isValid() const { return file.exists() && file.hasFileExtension(".mp3;.wav;.flac;.aac;.m4a"); }
    juce::String getFormattedDuration() const;
    
private:
    juce::File file;
    juce::String title;
    juce::String artist;
    juce::String album;
    juce::String genre;
    double duration = 0.0;
    int bpm = 0;
    
    void extractMetadata();
};
