//Juce Library
#include <JuceHeader.h>
#include <functional>
#include "WaveformDisplay.h"

#pragma once

class DJAudioPlayer : public AudioSource
{
public:
    //constructor
    DJAudioPlayer(AudioFormatManager& _formatManager,AudioThumbnailCache& cacheToUse);
    //Destructor
    ~DJAudioPlayer();
    
    //AudioSource interface
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    //equivalent to draw() in p5. It runs until app is quit
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    //called when app exits, used to free up audio resources
    void releaseResources() override;
    //====================================================================
    
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);
    
    void loadFile(File audioFile);
    
    void setGain(double gain);
    void setSpeed(double ratio);		    
    void start();
    void stop();
    
    /**Get relative position of the playerhead **/
    double getPositionRelative();
    
    //==============================================================
    //Callback
    using FileLoadedCallBack = std::function<void(const File&)>;
    /**Setter function for the file loaded callback**/
    void setFileLoadedCallback(FileLoadedCallBack callback);
    
    //testing
    void loadFileIntoWaveform(const juce::File& file);

    //////=========================
    // Add a method to get the current level
    float getCurrentRMSLevel() const { return currentRMSLevel; }
    void calculateCurrentLevel(const juce::AudioSourceChannelInfo& bufferToFill);
    
    bool isPlaying() const;
    double getCurrentPosition() const;
    double getLengthInSeconds() const;

    //==============================================================
    juce::Array<juce::File> loadedFiles; // Array to keep track of loaded files
    
private:
    
    // Add a member to store the current level
    float currentRMSLevel = 0.0f;
    
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource{&transportSource, false, 2};
    
    /**Callback function to notify PlaylistComponent about the loaded file**/
    FileLoadedCallBack fileLoadedCallback;

    juce::File currentLoadedFile; // Track the currently loaded file
    
    WaveformDisplay wave;
};

