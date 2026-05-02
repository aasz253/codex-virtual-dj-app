#include "ModernSlider.h"

ModernSlider::ModernSlider(Style sliderStyle)
    : currentStyle(sliderStyle),
      accentColor(juce::Colour(0xff00d4ff)),
      backgroundColor(juce::Colour(0xff2a2a2a)),
      textColor(juce::Colours::white)
{
    // Set the appropriate JUCE slider style based on our custom style
    switch (currentStyle)
    {
        case Style::Vertical:
        case Style::EQ:
            Slider::setSliderStyle(Slider::LinearVertical);
            break;
        case Style::Horizontal:
            Slider::setSliderStyle(Slider::LinearHorizontal);
            break;
        case Style::Rotary:
            Slider::setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
            break;
    }
    
    setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    setColour(Slider::thumbColourId, accentColor);
    setColour(Slider::trackColourId, backgroundColor);
    setColour(Slider::backgroundColourId, backgroundColor.darker());
}

ModernSlider::~ModernSlider()
{
}

void ModernSlider::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    switch (currentStyle)
    {
        case Style::Vertical:
            paintVerticalSlider(g, bounds);
            break;
        case Style::Horizontal:
            paintHorizontalSlider(g, bounds);
            break;
        case Style::Rotary:
            paintRotarySlider(g, bounds);
            break;
        case Style::EQ:
            paintEQSlider(g, bounds);
            break;
    }
}

void ModernSlider::setSliderStyle(Style newStyle)
{
    currentStyle = newStyle;
    
    switch (currentStyle)
    {
        case Style::Vertical:
        case Style::EQ:
            Slider::setSliderStyle(Slider::LinearVertical);
            break;
        case Style::Horizontal:
            Slider::setSliderStyle(Slider::LinearHorizontal);
            break;
        case Style::Rotary:
            Slider::setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
            break;
    }
    
    repaint();
}

void ModernSlider::paintVerticalSlider(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto sliderBounds = getSliderBounds();
    auto textBounds = getTextBounds();
    
    // Background track
    g.setColour(backgroundColor);
    auto trackWidth = 4.0f;
    auto trackBounds = sliderBounds.reduced((sliderBounds.getWidth() - trackWidth) / 2, 0).toFloat();
    g.fillRoundedRectangle(trackBounds, trackWidth / 2);
    
    // Value track
    auto value = getValue();
    auto proportion = (value - getMinimum()) / (getMaximum() - getMinimum());
    auto valueHeight = trackBounds.getHeight() * (1.0f - proportion);
    auto valueTrack = trackBounds.removeFromBottom(trackBounds.getHeight() - valueHeight);
    
    g.setColour(accentColor);
    g.fillRoundedRectangle(valueTrack, trackWidth / 2);
    
    // Thumb
    auto thumbY = trackBounds.getY() + valueHeight - 6;
    auto thumbBounds = juce::Rectangle<float>(trackBounds.getCentreX() - 8, thumbY, 16, 12);
    
    g.setColour(accentColor.brighter(0.3f));
    g.fillRoundedRectangle(thumbBounds, 2.0f);
    
    // Border
    g.setColour(accentColor);
    g.drawRoundedRectangle(thumbBounds, 2.0f, 1.0f);
    
    // Label and value text
    if (showLabel && labelText.isNotEmpty())
    {
        g.setColour(textColor);
        g.setFont(12.0f);
        g.drawText(labelText, textBounds.removeFromTop(15), juce::Justification::centred);
    }
    
    if (showValue)
    {
        g.setColour(textColor.withAlpha(0.8f));
        g.setFont(10.0f);
        auto valueText = juce::String(value, 2, false);
        g.drawText(valueText, textBounds.removeFromBottom(15), juce::Justification::centred);
    }
}

void ModernSlider::paintHorizontalSlider(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto sliderBounds = getSliderBounds();
    
    // Background track
    g.setColour(backgroundColor);
    auto trackHeight = 4.0f;
    auto trackBounds = sliderBounds.reduced(0, (sliderBounds.getHeight() - trackHeight) / 2).toFloat();
    g.fillRoundedRectangle(trackBounds, trackHeight / 2);
    
    // Value track
    auto value = getValue();
    auto proportion = (value - getMinimum()) / (getMaximum() - getMinimum());
    auto valueWidth = trackBounds.getWidth() * proportion;
    auto valueTrack = trackBounds.removeFromLeft(valueWidth);
    
    g.setColour(accentColor);
    g.fillRoundedRectangle(valueTrack, trackHeight / 2);
    
    // Thumb
    auto thumbX = trackBounds.getX() + valueWidth - 6;
    auto thumbBounds = juce::Rectangle<float>(thumbX, trackBounds.getCentreY() - 8, 12, 16);
    
    g.setColour(accentColor.brighter(0.3f));
    g.fillRoundedRectangle(thumbBounds, 2.0f);
    
    g.setColour(accentColor);
    g.drawRoundedRectangle(thumbBounds, 2.0f, 1.0f);
}

void ModernSlider::paintRotarySlider(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto sliderBounds = getSliderBounds().toFloat();
    auto centre = sliderBounds.getCentre();
    auto radius = juce::jmin(sliderBounds.getWidth(), sliderBounds.getHeight()) / 2.0f - 4.0f;
    
    // Background circle
    g.setColour(backgroundColor);
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);
    
    // Value arc
    auto value = getValue();
    auto proportion = (value - getMinimum()) / (getMaximum() - getMinimum());
    auto startAngle = juce::MathConstants<float>::pi * 1.25f;
    auto endAngle = startAngle + juce::MathConstants<float>::pi * 1.5f * proportion;
    
    juce::Path valueArc;
    valueArc.addCentredArc(centre.x, centre.y, radius - 2, radius - 2, 0.0f, startAngle, endAngle, true);
    
    g.setColour(accentColor);
    g.strokePath(valueArc, juce::PathStrokeType(3.0f));
    
    // Thumb
    auto thumbAngle = startAngle + (endAngle - startAngle);
    auto thumbX = centre.x + std::cos(thumbAngle - juce::MathConstants<float>::halfPi) * (radius - 2);
    auto thumbY = centre.y + std::sin(thumbAngle - juce::MathConstants<float>::halfPi) * (radius - 2);
    
    g.setColour(accentColor.brighter(0.3f));
    g.fillEllipse(thumbX - 4, thumbY - 4, 8, 8);
}

void ModernSlider::paintEQSlider(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto sliderBounds = getSliderBounds();
    auto textBounds = getTextBounds();
    
    // Background track
    g.setColour(backgroundColor);
    auto trackWidth = 6.0f;
    auto trackBounds = sliderBounds.reduced((sliderBounds.getWidth() - trackWidth) / 2, 0).toFloat();
    g.fillRoundedRectangle(trackBounds, trackWidth / 2);
    
    // Center line (0dB)
    auto centerY = trackBounds.getCentreY();
    g.setColour(textColor.withAlpha(0.3f));
    g.drawLine(trackBounds.getX(), centerY, trackBounds.getRight(), centerY, 1.0f);
    
    // Value indicator
    auto value = getValue();
    auto range = getMaximum() - getMinimum();
    auto proportion = (value - getMinimum()) / range;
    auto valueY = trackBounds.getBottom() - (trackBounds.getHeight() * proportion);
    
    // Draw value area from center
    juce::Rectangle<float> valueArea;
    if (value >= 0)
    {
        valueArea = juce::Rectangle<float>(trackBounds.getX(), valueY, trackBounds.getWidth(), centerY - valueY);
        g.setColour(accentColor.withAlpha(0.7f));
    }
    else
    {
        valueArea = juce::Rectangle<float>(trackBounds.getX(), centerY, trackBounds.getWidth(), valueY - centerY);
        g.setColour(juce::Colours::orange.withAlpha(0.7f));
    }
    g.fillRoundedRectangle(valueArea, trackWidth / 2);
    
    // Thumb
    auto thumbBounds = juce::Rectangle<float>(trackBounds.getCentreX() - 10, valueY - 4, 20, 8);
    g.setColour(value >= 0 ? accentColor : juce::Colours::orange);
    g.fillRoundedRectangle(thumbBounds, 2.0f);
    
    // Label and value
    if (showLabel && labelText.isNotEmpty())
    {
        g.setColour(textColor);
        g.setFont(10.0f);
        g.drawText(labelText, textBounds.removeFromTop(12), juce::Justification::centred);
    }
    
    if (showValue)
    {
        g.setColour(textColor.withAlpha(0.8f));
        g.setFont(9.0f);
        auto valueText = juce::String(value, 1, false) + "dB";
        g.drawText(valueText, textBounds.removeFromBottom(12), juce::Justification::centred);
    }
}

juce::Rectangle<int> ModernSlider::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    if (showLabel && labelText.isNotEmpty())
        bounds.removeFromTop(15);
    
    if (showValue)
        bounds.removeFromBottom(15);
    
    return bounds.reduced(4);
}

juce::Rectangle<int> ModernSlider::getTextBounds() const
{
    return getLocalBounds();
}