#pragma once
#include <JuceHeader.h>
#include "../Model/Track.h"

class WaveformView : public juce::Component,
                    public juce::ChangeListener,
                    public juce::Timer
{
public:
    WaveformView(juce::AudioFormatManager& formatManager, juce::AudioThumbnailCache& cache);
    ~WaveformView() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    
    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    // Timer
    void timerCallback() override;
    
    // Public methods
    void loadTrack(const Track& track);
    void setPosition(double position); // 0.0 to 1.0
    void setZoom(double zoomLevel); // 1.0 = full track, higher = zoomed in
    void setPlaybackPosition(double position);
    
    // Cue points
    void setCuePoint(double position);
    void clearCuePoint();
    
    // Loop points
    void setLoopStart(double position);
    void setLoopEnd(double position);
    void clearLoop();
    
    // Callbacks
    std::function<void(double)> onPositionChanged;
    std::function<void(double)> onCuePointSet;
    std::function<void(double, double)> onLoopSet;
    
    // Visual settings
    void setWaveformColor(juce::Colour color) { waveformColor = color; repaint(); }
    void setBackgroundColor(juce::Colour color) { backgroundColor = color; repaint(); }
    void setPositionColor(juce::Colour color) { positionColor = color; repaint(); }
    void setCueColor(juce::Colour color) { cueColor = color; repaint(); }
    void setLoopColor(juce::Colour color) { loopColor = color; repaint(); }
    
private:
    juce::AudioFormatManager& formatManager;
    juce::AudioThumbnail audioThumbnail;
    
    // Current state
    double currentPosition = 0.0;
    double playbackPosition = 0.0;
    double zoomLevel = 1.0;
    double viewStart = 0.0;
    
    // Cue and loop points
    double cuePoint = -1.0;
    double loopStart = -1.0;
    double loopEnd = -1.0;
    
    // Colors
    juce::Colour waveformColor;
    juce::Colour backgroundColor;
    juce::Colour positionColor;
    juce::Colour cueColor;
    juce::Colour loopColor;
    
    // Interaction
    bool isDragging = false;
    
    // Helper methods
    double pixelToTime(int pixel) const;
    int timeToPixel(double time) const;
    void updateViewForZoom();
    void drawWaveform(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPositionMarker(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawCuePoint(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawLoopRegion(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTimeScale(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformView)
};