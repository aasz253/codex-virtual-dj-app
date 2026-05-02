#pragma once
#include <JuceHeader.h>

class JogWheel : public juce::Component,
                public juce::Timer
{
public:
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void jogWheelMoved(JogWheel* wheel, float deltaAngle, bool isTouched) = 0;
        virtual void jogWheelPressed(JogWheel* wheel, bool isPressed) = 0;
        virtual void jogWheelDoubleClicked(JogWheel* wheel) = 0;
    };
    
    JogWheel();
    ~JogWheel() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse handling
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    
    // Timer
    void timerCallback() override;
    
    // Listener management
    void addListener(Listener* listener);
    void removeListener(Listener* listener);
    
    // Wheel state
    void setRotation(float angleInRadians);
    float getRotation() const { return currentRotation; }
    void setTouched(bool touched);
    bool isTouched() const { return isCurrentlyTouched; }
    
    // Visual settings
    void setWheelColor(juce::Colour color) { wheelColor = color; repaint(); }
    void setRimColor(juce::Colour color) { rimColor = color; repaint(); }
    void setDotColor(juce::Colour color) { dotColor = color; repaint(); }
    void setTouchColor(juce::Colour color) { touchColor = color; repaint(); }
    void setBackgroundColor(juce::Colour color) { backgroundColor = color; repaint(); }
    
    // Sensitivity settings
    void setSensitivity(float sensitivity) { this->sensitivity = sensitivity; }
    void setInertia(float inertia) { this->inertia = inertia; }
    
    // Display settings
    void setShowDots(bool show) { showDots = show; repaint(); }
    void setShowRPM(bool show) { showRPM = show; repaint(); }
    void setRPM(float rpm) { currentRPM = rpm; }
    
private:
    // Listeners
    juce::ListenerList<Listener> listeners;
    
    // State
    float currentRotation = 0.0f;
    float targetRotation = 0.0f;
    bool isCurrentlyTouched = false;
    bool isMouseOver = false;
    bool isDragging = false;
    
    // Mouse tracking
    juce::Point<float> lastMousePos;
    juce::Point<float> wheelCenter;
    float lastAngle = 0.0f;
    
    // Animation
    float rotationVelocity = 0.0f;
    float smoothingFactor = 0.85f;
    
    // Visual properties
    juce::Colour wheelColor;
    juce::Colour rimColor;
    juce::Colour dotColor;
    juce::Colour touchColor;
    juce::Colour backgroundColor;
    
    // Settings
    float sensitivity = 1.0f;
    float inertia = 0.95f;
    bool showDots = true;
    bool showRPM = true;
    float currentRPM = 0.0f;
    
    // Visual feedback
    float touchIntensity = 0.0f;
    float hoverIntensity = 0.0f;
    
    // Helper methods
    float getAngleFromPoint(juce::Point<float> point) const;
    float normalizeAngle(float angle) const;
    void updateRotation(float newAngle);
    void drawWheel(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawRim(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawDots(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawCenterButton(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawRPMDisplay(juce::Graphics& g, juce::Rectangle<float> bounds);
    void notifyListeners(float deltaAngle);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JogWheel)
};