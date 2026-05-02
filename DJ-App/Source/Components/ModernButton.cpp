#include "ModernButton.h"

ModernButton::ModernButton(const juce::String& buttonText, Style buttonStyle)
    : Button(buttonText),
      currentStyle(buttonStyle),
      accentColor(juce::Colour(0xff00d4ff)),
      backgroundColor(juce::Colour(0xff2a2a2a)),
      textColor(juce::Colours::white)
{
    setClickingTogglesState(currentStyle == Style::Toggle);
}

ModernButton::~ModernButton()
{
}

void ModernButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds();
    
    switch (currentStyle)
    {
        case Style::Primary:
            paintPrimaryButton(g, bounds, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            break;
        case Style::Secondary:
            paintSecondaryButton(g, bounds, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            break;
        case Style::Toggle:
            paintToggleButton(g, bounds, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            break;
        case Style::Icon:
            paintIconButton(g, bounds, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            break;
        case Style::Danger:
            paintDangerButton(g, bounds, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            break;
    }
}

void ModernButton::setButtonStyle(Style newStyle)
{
    currentStyle = newStyle;
    setClickingTogglesState(currentStyle == Style::Toggle);
    repaint();
}

void ModernButton::paintPrimaryButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down)
{
    auto buttonColor = getButtonColor(highlighted, down);
    auto textColorToUse = getTextColorForState(highlighted, down);
    
    // Background
    g.setColour(buttonColor);
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    // Highlight border
    if (highlighted || down)
    {
        g.setColour(accentColor.brighter(0.3f));
        g.drawRoundedRectangle(bounds.toFloat().reduced(1), 4.0f, 1.0f);
    }
    
    // Text
    g.setColour(textColorToUse);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}

void ModernButton::paintSecondaryButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down)
{
    auto borderColor = highlighted || down ? accentColor : backgroundColor.brighter(0.3f);
    auto fillColor = down ? backgroundColor.brighter(0.2f) : (highlighted ? backgroundColor.brighter(0.1f) : juce::Colours::transparentBlack);
    auto textColorToUse = getTextColorForState(highlighted, down);
    
    // Background
    if (!fillColor.isTransparent())
    {
        g.setColour(fillColor);
        g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    }
    
    // Border
    g.setColour(borderColor);
    g.drawRoundedRectangle(bounds.toFloat().reduced(1), 4.0f, 1.5f);
    
    // Text
    g.setColour(textColorToUse);
    g.setFont(14.0f);
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}

void ModernButton::paintToggleButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down)
{
    bool isToggled = getToggleState();
    auto buttonColor = isToggled ? accentColor : (highlighted ? backgroundColor.brighter(0.2f) : backgroundColor);
    auto textColorToUse = isToggled ? juce::Colours::black : textColor;
    
    // Background
    g.setColour(buttonColor);
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    // Border for non-toggled state
    if (!isToggled)
    {
        g.setColour(backgroundColor.brighter(0.3f));
        g.drawRoundedRectangle(bounds.toFloat().reduced(1), 4.0f, 1.0f);
    }
    
    // Glow effect when toggled
    if (isToggled)
    {
        g.setColour(accentColor.withAlpha(0.3f));
        g.drawRoundedRectangle(bounds.toFloat().expanded(2), 6.0f, 2.0f);
    }
    
    // Text
    g.setColour(textColorToUse);
    g.setFont(juce::Font(14.0f, isToggled ? juce::Font::bold : juce::Font::plain));
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}

void ModernButton::paintIconButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down)
{
    auto buttonColor = down ? backgroundColor.brighter(0.3f) : (highlighted ? backgroundColor.brighter(0.1f) : juce::Colours::transparentBlack);
    auto iconColor = highlighted || down ? accentColor : textColor;
    
    // Background (subtle)
    if (!buttonColor.isTransparent())
    {
        g.setColour(buttonColor);
        g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    }
    
    // Icon
    if (hasIcon)
    {
        auto iconBounds = bounds.reduced(4).toFloat();
        auto iconArea = iconBounds.withSizeKeepingCentre(iconSize, iconSize);
        
        g.setColour(iconColor);
        g.fillPath(icon, icon.getTransformToScaleToFit(iconArea, true));
    }
    
    // Text below icon if both present
    if (hasIcon && getButtonText().isNotEmpty())
    {
        auto textBounds = bounds.removeFromBottom(20);
        g.setColour(iconColor);
        g.setFont(10.0f);
        g.drawText(getButtonText(), textBounds, juce::Justification::centred);
    }
    else if (getButtonText().isNotEmpty())
    {
        g.setColour(iconColor);
        g.setFont(14.0f);
        g.drawText(getButtonText(), bounds, juce::Justification::centred);
    }
}

void ModernButton::paintDangerButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down)
{
    auto dangerColor = juce::Colour(0xffff4444);
    auto buttonColor = down ? dangerColor.darker(0.2f) : (highlighted ? dangerColor.brighter(0.1f) : dangerColor);
    
    // Background
    g.setColour(buttonColor);
    g.fillRoundedRectangle(bounds.toFloat(), 4.0f);
    
    // Highlight border
    if (highlighted || down)
    {
        g.setColour(dangerColor.brighter(0.3f));
        g.drawRoundedRectangle(bounds.toFloat().reduced(1), 4.0f, 1.0f);
    }
    
    // Text
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}

juce::Colour ModernButton::getButtonColor(bool highlighted, bool down) const
{
    if (down)
        return accentColor.darker(0.2f);
    else if (highlighted)
        return accentColor.brighter(0.1f);
    else
        return accentColor;
}

juce::Colour ModernButton::getTextColorForState(bool highlighted, bool down) const
{
    switch (currentStyle)
    {
        case Style::Primary:
        case Style::Danger:
            return juce::Colours::white;
        case Style::Secondary:
        case Style::Icon:
            return highlighted || down ? accentColor : textColor;
        case Style::Toggle:
            return getToggleState() ? juce::Colours::black : textColor;
        default:
            return textColor;
    }
}