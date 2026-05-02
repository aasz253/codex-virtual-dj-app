#pragma once
#include <JuceHeader.h>
#include "../Model/AudioEngine.h"
#include "../Model/PlaylistManager.h"
#include "../Model/Track.h"
#include <memory>

class DJController : public juce::AudioAppComponent
{
public:
    DJController();
    ~DJController() override;
    
    // AudioAppComponent interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    // Deck control
    void loadTrackToDeck(int deckIndex, const Track& track);
    void playDeck(int deckIndex);
    void pauseDeck(int deckIndex);
    void stopDeck(int deckIndex);
    void setDeckPosition(int deckIndex, double position);
    void setDeckGain(int deckIndex, double gain);
    void setDeckSpeed(int deckIndex, double speed);
    void setDeckEQ(int deckIndex, double low, double mid, double high);
    
    // Additional deck controls
    void togglePlay(int deckIndex);
    void cue(int deckIndex);
    void loadTrack(int deckIndex);
    void toggleSync(int deckIndex);
    void toggleLoop(int deckIndex);
    void adjustPosition(int deckIndex, double delta);
    void adjustSpeed(int deckIndex, double delta);
    void stop(int deckIndex);
    
    // Crossfader and mixer
    void setCrossfader(double position); // -1.0 (deck 1) to 1.0 (deck 2)
    void setMasterGain(double gain);
    void setCueGain(double gain);
    
    // Playlist management
    PlaylistManager& getPlaylistManager() { return playlistManager; }
    void loadPlaylistFromDirectory(const juce::File& directory);
    void loadPlaylistFromFiles(const juce::Array<juce::File>& files);
    
    // Audio engines access
    AudioEngine& getDeck1() { return *deck1; }
    AudioEngine& getDeck2() { return *deck2; }
    
    // Audio format manager access
    juce::AudioFormatManager& getFormatManager() { return formatManager; }
    juce::AudioThumbnailCache& getThumbnailCache() { return thumbnailCache; }
    
    // BPM sync and beat matching
    void syncDecks();
    void enableBeatSync(bool enable);
    bool isBeatSyncEnabled() const { return beatSyncEnabled; }
    
    // Recording
    void startRecording(const juce::File& outputFile);
    void stopRecording();
    bool isRecording() const;
    
    // Auto-mix features
    void enableAutoCrossfade(bool enable);
    void setAutoCrossfadeTime(double seconds);
    
    // Callbacks for UI updates
    std::function<void(int, double)> onDeckPositionChanged;
    std::function<void(int, float, float)> onDeckLevelsChanged; // RMS, Peak
    std::function<void(double)> onCrossfaderChanged;
    std::function<void()> onPlaylistChanged;
    
private:
    // Audio components
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    
    // Audio engines
    std::unique_ptr<AudioEngine> deck1;
    std::unique_ptr<AudioEngine> deck2;
    
    // Mixer
    juce::MixerAudioSource mixerSource;
    
    // Playlist
    PlaylistManager playlistManager;
    
    // Mixer state
    double crossfaderPosition = 0.0; // -1.0 to 1.0
    double masterGain = 0.8;
    double cueGain = 0.8;
    
    // Beat sync
    bool beatSyncEnabled = false;
    
    // Recording
    std::unique_ptr<juce::AudioFormatWriter> recordingWriter;
    juce::File recordingFile;
    
    // Auto-mix
    bool autoCrossfadeEnabled = false;
    double autoCrossfadeTime = 10.0;
    
    // Internal methods
    void setupAudioEngines();
    void updateMixerLevels();
    double calculateDeckGain(int deckIndex) const;
    void handleDeckPositionChange(int deckIndex, double position);
    void handleDeckLevelsChange(int deckIndex);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DJController)
};