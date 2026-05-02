#include "Track.h"

Track::Track(const juce::File& audioFile) : file(audioFile)
{
    if (file.exists())
    {
        extractMetadata();
    }
}

void Track::extractMetadata()
{
    // Set default values from filename if metadata extraction fails
    title = file.getFileNameWithoutExtension();
    artist = "Unknown Artist";
    album = "Unknown Album";
    genre = "Unknown";
    
    // Try to extract metadata using JUCE's audio format manager
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        duration = reader->lengthInSamples / reader->sampleRate;
        
        // Extract metadata if available
        auto metadata = reader->metadataValues;
        
        if (metadata.containsKey("title") && metadata["title"].isNotEmpty())
            title = metadata["title"];
        if (metadata.containsKey("artist") && metadata["artist"].isNotEmpty())
            artist = metadata["artist"];
        if (metadata.containsKey("album") && metadata["album"].isNotEmpty())
            album = metadata["album"];
        if (metadata.containsKey("genre") && metadata["genre"].isNotEmpty())
            genre = metadata["genre"];
        
        delete reader;
    }
}

juce::String Track::getFormattedDuration() const
{
    int minutes = static_cast<int>(duration) / 60;
    int seconds = static_cast<int>(duration) % 60;
    return juce::String::formatted("%d:%02d", minutes, seconds);
}