#pragma once
#include <JuceHeader.h>
#include "../Controller/DJController.h"
#include "../Components/ModernSlider.h"
#include "../Components/ModernButton.h"
#include "../Components/WaveformView.h"
#include "../Components/VUMeter.h"
#include "../Components/JogWheel.h"

class DeckView : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener,
                public juce::Timer,
                public JogWheel::Listener,
                public juce::FileDragAndDropTarget
{
public:
    DeckView(DJController& controller, int deckIndex);
    ~DeckView() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Button::Listener
    void buttonClicked(juce::Button* button) override;
    
    // Slider::Listener
    void sliderValueChanged(juce::Slider* slider) override;
    
    // Timer
    void timerCallback() override;
    

    
    // JogWheel::Listener
    void jogWheelMoved(JogWheel* wheel, float deltaAngle, bool isTouched) override;
    void jogWheelPressed(JogWheel* wheel, bool isPressed) override;
    void jogWheelDoubleClicked(JogWheel* wheel) override;
    
    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragMove(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    
    // Public methods
    void updateTrackInfo(const juce::String& title, const juce::String& artist, double length);
    void updatePosition(double position);
    void updatePlayState(bool playing, bool paused);
    void updateLevels(float rms, float peak);
    void updateBPM(float bpm);
    void loadTrack(const juce::File& file);
    
private:
    DJController& djController;
    int deckIndex;
    
    // UI Components
    std::unique_ptr<ModernButton> playButton;
    std::unique_ptr<ModernButton> cueButton;
    std::unique_ptr<ModernButton> stopButton;
    std::unique_ptr<ModernButton> loadButton;
    std::unique_ptr<ModernButton> syncButton;
    std::unique_ptr<ModernButton> loopButton;
    
    std::unique_ptr<ModernSlider> gainSlider;
    std::unique_ptr<ModernSlider> speedSlider;
    std::unique_ptr<ModernSlider> lowEQSlider;
    std::unique_ptr<ModernSlider> midEQSlider;
    std::unique_ptr<ModernSlider> highEQSlider;
    
    std::unique_ptr<WaveformView> waveformDisplay;
    std::unique_ptr<VUMeter> vuMeter;
    std::unique_ptr<JogWheel> jogWheel;
    
    // Track info display
    juce::Label trackTitleLabel;
    juce::Label trackArtistLabel;
    juce::Label trackTimeLabel;
    juce::Label trackBPMLabel;
    
    // Colors and styling
    juce::Colour primaryColor;
    juce::Colour secondaryColor;
    juce::Colour accentColor;
    juce::Colour backgroundColor;
    
    // State
    bool isPlaying = false;
    bool isPaused = false;
    double currentPosition = 0.0;
    double trackLength = 0.0;
    float rmsLevel = 0.0f;
    float peakLevel = 0.0f;
    bool isDragOver = false;
    
    void setupComponents();
    void setupColors();
    void setupLayout();
    void updatePlayButton();
    juce::String formatTime(double seconds);
    void updateBPM(double bpm);
    void updateTimeDisplays();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckView)
};