#pragma once
#include <JuceHeader.h>
#include "Track.h"
#include <functional>

class AudioEngine : public juce::AudioSource,
                   public juce::ChangeListener
{
public:
    AudioEngine(juce::AudioFormatManager& formatManager);
    ~AudioEngine() override;
    
    // AudioSource interface
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    // ChangeListener interface
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    // Playback control
    void loadTrack(const Track& track);
    void play();
    void pause();
    void stop();
    bool isPlaying() const;
    bool isPaused() const;
    
    // Position control
    void setPosition(double positionInSeconds);
    void setPositionRelative(double position); // 0.0 to 1.0
    double getPosition() const;
    double getPositionRelative() const;
    double getTrackLength() const;
    
    // Audio effects
    void setGain(double gain); // 0.0 to 2.0
    void setSpeed(double speed); // 0.5 to 2.0
    void setPitch(double pitch); // -12 to +12 semitones
    void setLowEQ(double gain); // -20dB to +20dB
    void setMidEQ(double gain);
    void setHighEQ(double gain);
    
    // Getters for current values
    double getGain() const { return currentGain; }
    double getSpeed() const { return currentSpeed; }
    double getPitch() const { return currentPitch; }
    double getLowEQ() const { return lowEQGain; }
    double getMidEQ() const { return midEQGain; }
    double getHighEQ() const { return highEQGain; }
    
    // Audio analysis
    float getRMSLevel() const { return rmsLevel; }
    float getPeakLevel() const { return peakLevel; }
    
    // Cue points
    void setCuePoint(double position);
    void jumpToCue();
    double getCuePoint() const { return cuePoint; }
    
    // Loop functionality
    void setLoopStart(double position);
    void setLoopEnd(double position);
    void enableLoop(bool enable);
    bool isLoopEnabled() const { return loopEnabled; }
    
    // Callbacks
    std::function<void()> onTrackLoaded;
    std::function<void()> onPlaybackStarted;
    std::function<void()> onPlaybackStopped;
    std::function<void(double)> onPositionChanged;
    
    // Current track
    const Track* getCurrentTrack() const { return currentTrack.get(); }
    
private:
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampleSource;
    
    // Audio effects chain
    juce::IIRFilter lowEQFilter, midEQFilter, highEQFilter;
    
    // Current state
    std::unique_ptr<Track> currentTrack;
    double currentGain = 1.0;
    double currentSpeed = 1.0;
    double currentPitch = 0.0;
    double lowEQGain = 0.0;
    double midEQGain = 0.0;
    double highEQGain = 0.0;
    
    // Audio analysis
    float rmsLevel = 0.0f;
    float peakLevel = 0.0f;
    
    // Cue and loop
    double cuePoint = 0.0;
    double loopStart = 0.0;
    double loopEnd = 0.0;
    bool loopEnabled = false;
    
    // Internal methods
    void updateAudioLevels(const juce::AudioSourceChannelInfo& bufferToFill);
    void applyEQ(juce::AudioBuffer<float>& buffer);
    void checkLoopBounds();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};