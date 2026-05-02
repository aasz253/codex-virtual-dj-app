#pragma once
#include <JuceHeader.h>
#include "DJAudioPlayer.h"

class DJDisk : public juce::Component, public juce::Timer 
{
public:
    DJDisk(DJAudioPlayer* player);
    ~DJDisk();

    void setTargetAngle(float angle);
    
    void timerCallback() override;
    
    bool keyPressed(const juce::KeyPress& key) override;

    void updatePlaybackPosition();

    void mouseDown(const juce::MouseEvent& event) override;
    
    void mouseDrag(const juce::MouseEvent& event) override;
    
    void mouseUp(const juce::MouseEvent& event) override;
    
    void paint(juce::Graphics& g) override;
    
private:
    float currentAngle; // Current visual angle of the disk
    float targetAngle; // Target angle where we want the disk to be
    
    float dragStartAngle;// Angle of the disk at the start of a drag operation
    juce::Point<float> dragStartPos;// Starting position of a mouse drag
    bool isDragging = false;// Flag to track whether the disk is being dragged
    
    DJAudioPlayer* player;// Pointer to the associated DJAudioPlayer
};

