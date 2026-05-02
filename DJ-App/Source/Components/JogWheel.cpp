#include "JogWheel.h"

JogWheel::JogWheel()
    : wheelColor(juce::Colour(0xff2a2a2a)),
      rimColor(juce::Colour(0xff404040)),
      dotColor(juce::Colour(0xff808080)),
      touchColor(juce::Colour(0xff00aaff)),
      backgroundColor(juce::Colour(0xff1a1a1a))
{
    startTimerHz(60); // 60 FPS for smooth animation
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}

JogWheel::~JogWheel()
{
    stopTimer();
}

void JogWheel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Fill background
    g.setColour(backgroundColor);
    g.fillEllipse(bounds);
    
    // Calculate wheel bounds (leave space for rim)
    auto wheelBounds = bounds.reduced(8.0f);
    wheelCenter = wheelBounds.getCentre();
    
    // Apply rotation transform for visual spinning effect
    g.saveState();
    g.addTransform(juce::AffineTransform::rotation(currentRotation, wheelCenter.x, wheelCenter.y));
    
    // Draw wheel components with rotation
    drawWheel(g, wheelBounds);
    drawRim(g, bounds);
    
    if (showDots)
        drawDots(g, wheelBounds);
    
    drawCenterButton(g, wheelBounds.reduced(wheelBounds.getWidth() * 0.3f));
    
    g.restoreState();
    
    if (showRPM)
        drawRPMDisplay(g, bounds);
}

void JogWheel::resized()
{
    auto bounds = getLocalBounds().toFloat();
    wheelCenter = bounds.getCentre();
}

void JogWheel::mouseDown(const juce::MouseEvent& event)
{
    if (event.mods.isLeftButtonDown())
    {
        isDragging = true;
        setTouched(true);
        lastMousePos = event.position;
        lastAngle = getAngleFromPoint(event.position);
        rotationVelocity = 0.0f; // Stop any inertia
        
        listeners.call([this](Listener& l) { l.jogWheelPressed(this, true); });
    }
}

void JogWheel::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        float currentAngle = getAngleFromPoint(event.position);
        float deltaAngle = currentAngle - lastAngle;
        
        // Handle angle wrap-around
        if (deltaAngle > juce::MathConstants<float>::pi)
            deltaAngle -= juce::MathConstants<float>::twoPi;
        else if (deltaAngle < -juce::MathConstants<float>::pi)
            deltaAngle += juce::MathConstants<float>::twoPi;
        
        // Apply sensitivity
        deltaAngle *= sensitivity;
        
        // Update rotation
        updateRotation(currentRotation + deltaAngle);
        
        // Calculate velocity for inertia
        rotationVelocity = deltaAngle * 0.5f;
        
        // Notify listeners
        notifyListeners(deltaAngle);
        
        lastAngle = currentAngle;
        lastMousePos = event.position;
    }
}

void JogWheel::mouseUp(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        isDragging = false;
        setTouched(false);
        
        listeners.call([this](Listener& l) { l.jogWheelPressed(this, false); });
    }
}

void JogWheel::mouseDoubleClick(const juce::MouseEvent& event)
{
    listeners.call([this](Listener& l) { l.jogWheelDoubleClicked(this); });
}

void JogWheel::mouseEnter(const juce::MouseEvent& event)
{
    isMouseOver = true;
}

void JogWheel::mouseExit(const juce::MouseEvent& event)
{
    isMouseOver = false;
}

void JogWheel::timerCallback()
{
    bool needsRepaint = false;
    
    // Animate touch intensity
    float targetTouchIntensity = isCurrentlyTouched ? 1.0f : 0.0f;
    if (std::abs(touchIntensity - targetTouchIntensity) > 0.01f)
    {
        touchIntensity += (targetTouchIntensity - touchIntensity) * 0.1f;
        needsRepaint = true;
    }
    
    // Animate hover intensity
    float targetHoverIntensity = isMouseOver ? 1.0f : 0.0f;
    if (std::abs(hoverIntensity - targetHoverIntensity) > 0.01f)
    {
        hoverIntensity += (targetHoverIntensity - hoverIntensity) * 0.15f;
        needsRepaint = true;
    }
    
    // Apply inertia when not being dragged
    if (!isDragging && std::abs(rotationVelocity) > 0.001f)
    {
        updateRotation(currentRotation + rotationVelocity);
        rotationVelocity *= inertia;
        
        // Notify listeners about inertia movement
        notifyListeners(rotationVelocity);
        needsRepaint = true;
    }
    
    // Smooth rotation animation
    if (std::abs(currentRotation - targetRotation) > 0.001f)
    {
        currentRotation += (targetRotation - currentRotation) * smoothingFactor;
        needsRepaint = true;
    }
    
    if (needsRepaint)
        repaint();
}

void JogWheel::addListener(Listener* listener)
{
    listeners.add(listener);
}

void JogWheel::removeListener(Listener* listener)
{
    listeners.remove(listener);
}

void JogWheel::setRotation(float angleInRadians)
{
    targetRotation = normalizeAngle(angleInRadians);
}

void JogWheel::setTouched(bool touched)
{
    isCurrentlyTouched = touched;
}

float JogWheel::getAngleFromPoint(juce::Point<float> point) const
{
    auto delta = point - wheelCenter;
    return std::atan2(delta.y, delta.x);
}

float JogWheel::normalizeAngle(float angle) const
{
    while (angle > juce::MathConstants<float>::pi)
        angle -= juce::MathConstants<float>::twoPi;
    while (angle < -juce::MathConstants<float>::pi)
        angle += juce::MathConstants<float>::twoPi;
    return angle;
}

void JogWheel::updateRotation(float newAngle)
{
    currentRotation = normalizeAngle(newAngle);
    targetRotation = currentRotation;
}

void JogWheel::drawWheel(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    // Base wheel color with touch/hover effects
    auto baseColor = wheelColor;
    if (touchIntensity > 0.0f)
        baseColor = baseColor.interpolatedWith(touchColor, touchIntensity * 0.3f);
    if (hoverIntensity > 0.0f)
        baseColor = baseColor.brighter(hoverIntensity * 0.1f);
    
    // Draw main wheel with gradient
    juce::ColourGradient gradient(baseColor.brighter(0.2f), bounds.getCentreX(), bounds.getY(),
                                 baseColor.darker(0.3f), bounds.getCentreX(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(bounds);
    
    // Draw inner shadow
    g.setColour(juce::Colour(0x40000000));
    g.drawEllipse(bounds.reduced(2.0f), 2.0f);
}

void JogWheel::drawRim(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto rimBounds = bounds.reduced(2.0f);
    
    // Outer rim
    g.setColour(rimColor.brighter(hoverIntensity * 0.2f));
    g.drawEllipse(rimBounds, 4.0f);
    
    // Inner rim with touch effect
    auto innerRimColor = rimColor;
    if (touchIntensity > 0.0f)
        innerRimColor = innerRimColor.interpolatedWith(touchColor, touchIntensity * 0.5f);
    
    g.setColour(innerRimColor);
    g.drawEllipse(bounds.reduced(8.0f), 2.0f);
}

void JogWheel::drawDots(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto center = bounds.getCentre();
    float radius = bounds.getWidth() * 0.35f;
    int numDots = 24;
    
    for (int i = 0; i < numDots; ++i)
    {
        float angle = (i * juce::MathConstants<float>::twoPi / numDots) + currentRotation;
        float x = center.x + radius * std::cos(angle);
        float y = center.y + radius * std::sin(angle);
        
        // Vary dot size and opacity based on position
        float dotSize = 2.0f + std::sin(angle * 4.0f) * 0.5f;
        float opacity = 0.6f + std::cos(angle * 2.0f) * 0.2f;
        
        auto dotColor = this->dotColor.withAlpha(opacity);
        if (touchIntensity > 0.0f)
            dotColor = dotColor.interpolatedWith(touchColor, touchIntensity * 0.4f);
        
        g.setColour(dotColor);
        g.fillEllipse(x - dotSize/2, y - dotSize/2, dotSize, dotSize);
    }
}

void JogWheel::drawCenterButton(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    // Center button with press effect
    auto buttonColor = wheelColor.brighter(0.3f);
    if (touchIntensity > 0.0f)
    {
        buttonColor = buttonColor.interpolatedWith(touchColor, touchIntensity * 0.6f);
        bounds = bounds.reduced(touchIntensity * 2.0f); // Press effect
    }
    
    // Button gradient
    juce::ColourGradient buttonGradient(buttonColor.brighter(0.3f), bounds.getCentreX(), bounds.getY(),
                                       buttonColor.darker(0.2f), bounds.getCentreX(), bounds.getBottom(), false);
    g.setGradientFill(buttonGradient);
    g.fillEllipse(bounds);
    
    // Button border
    g.setColour(rimColor);
    g.drawEllipse(bounds, 1.0f);
    
    // Center dot
    auto centerDot = bounds.reduced(bounds.getWidth() * 0.4f);
    g.setColour(dotColor.brighter(0.5f));
    g.fillEllipse(centerDot);
}

void JogWheel::drawRPMDisplay(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    if (!showRPM)
        return;
    
    // RPM text area
    auto textBounds = juce::Rectangle<float>(bounds.getCentreX() - 30, bounds.getBottom() - 25, 60, 20);
    
    g.setColour(juce::Colour(0x80000000));
    g.fillRoundedRectangle(textBounds, 3.0f);
    
    g.setColour(juce::Colour(0xffcccccc));
    g.setFont(12.0f);
    g.drawText(juce::String(currentRPM, 1, false) + " RPM", textBounds, juce::Justification::centred);
}

void JogWheel::notifyListeners(float deltaAngle)
{
    listeners.call([this, deltaAngle](Listener& l) { 
        l.jogWheelMoved(this, deltaAngle, isCurrentlyTouched); 
    });
}