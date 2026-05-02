#pragma once
#include <JuceHeader.h>
#include "../Components/ModernSlider.h"
#include "../Components/ModernButton.h"
#include "../Components/VUMeter.h"

class MixerView : public juce::Component,
                 public juce::Slider::Listener,
                 public juce::Button::Listener,
                 public juce::Timer
{
public:
    MixerView();
    ~MixerView() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Slider and button listeners
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    
    // Timer
    void timerCallback() override;
    
    // Update methods
    void updateMasterLevels(float rms, float peak);
    void updateCueLevels(float rms, float peak);
    void updateRecordingState(bool isRecording);
    
    // Callbacks
    std::function<void(float)> onCrossfaderChanged;
    std::function<void(float)> onMasterGainChanged;
    std::function<void(float)> onCueGainChanged;
    std::function<void(float)> onMasterFilterChanged;
    std::function<void(bool)> onRecordingToggled;
    std::function<void(bool)> onAutoMixToggled;
    std::function<void()> onBeatSyncPressed;
    
private:
    void setupComponents();
    void setupLayout();
    
    // Crossfader
    std::unique_ptr<ModernSlider> crossfaderSlider;
    std::unique_ptr<juce::Label> crossfaderLabel;
    
    // Master section
    std::unique_ptr<ModernSlider> masterGainSlider;
    std::unique_ptr<ModernSlider> masterFilterSlider;
    std::unique_ptr<VUMeter> masterVUMeter;
    std::unique_ptr<juce::Label> masterLabel;
    
    // Cue section
    std::unique_ptr<ModernSlider> cueGainSlider;
    std::unique_ptr<VUMeter> cueVUMeter;
    std::unique_ptr<juce::Label> cueLabel;
    
    // Control buttons
    std::unique_ptr<ModernButton> recordButton;
    std::unique_ptr<ModernButton> autoMixButton;
    std::unique_ptr<ModernButton> beatSyncButton;
    
    // Level values
    float masterRMSLevel = 0.0f;
    float masterPeakLevel = 0.0f;
    float cueRMSLevel = 0.0f;
    float cuePeakLevel = 0.0f;
    
    // State
    bool isRecording = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerView)
};