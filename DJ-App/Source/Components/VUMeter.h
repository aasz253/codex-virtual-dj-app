#pragma once
#include <JuceHeader.h>

class VUMeter : public juce::Component,
               public juce::Timer
{
public:
    enum class Orientation
    {
        Vertical,
        Horizontal
    };
    
    VUMeter(Orientation orientation = Orientation::Vertical);
    ~VUMeter() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Timer
    void timerCallback() override;
    
    // Level setting
    void setLevel(float rmsLevel, float peakLevel);
    void setRMSLevel(float level) { targetRMSLevel = level; }
    void setPeakLevel(float level) { targetPeakLevel = level; }
    
    // Visual settings
    void setOrientation(Orientation newOrientation);
    void setColors(juce::Colour green, juce::Colour yellow, juce::Colour red);
    void setBackgroundColor(juce::Colour color) { backgroundColor = color; repaint(); }
    void setShowPeakHold(bool show) { showPeakHold = show; }
    void setShowRMS(bool show) { showRMS = show; }
    
    // Calibration
    void setRange(float minDb, float maxDb);
    void setRedZone(float dbLevel) { redZoneDb = dbLevel; }
    void setYellowZone(float dbLevel) { yellowZoneDb = dbLevel; }
    
private:
    Orientation orientation;
    
    // Level values
    float currentRMSLevel = 0.0f;
    float currentPeakLevel = 0.0f;
    float targetRMSLevel = 0.0f;
    float targetPeakLevel = 0.0f;
    float peakHoldLevel = 0.0f;
    int peakHoldTimer = 0;
    
    // Visual settings
    juce::Colour greenColor;
    juce::Colour yellowColor;
    juce::Colour redColor;
    juce::Colour backgroundColor;
    bool showPeakHold = true;
    bool showRMS = true;
    
    // Range settings
    float minDb = -60.0f;
    float maxDb = 6.0f;
    float redZoneDb = -6.0f;
    float yellowZoneDb = -18.0f;
    
    // Animation
    float smoothingFactor = 0.8f;
    
    // Helper methods
    float levelToDb(float level) const;
    float dbToPosition(float db) const;
    juce::Colour getColorForLevel(float db) const;
    void drawVerticalMeter(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawHorizontalMeter(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawScale(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeter)
};