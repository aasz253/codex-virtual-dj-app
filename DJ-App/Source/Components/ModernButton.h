#pragma once
#include <JuceHeader.h>

class ModernButton : public juce::Button
{
public:
    enum class Style
    {
        Primary,
        Secondary,
        Toggle,
        Icon,
        Danger
    };
    
    ModernButton(const juce::String& buttonText = {}, Style buttonStyle = Style::Primary);
    ~ModernButton() override;
    
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    void setButtonStyle(Style newStyle);
    void setAccentColor(juce::Colour color) { accentColor = color; repaint(); }
    void setBackgroundColor(juce::Colour color) { backgroundColor = color; repaint(); }
    void setTextColor(juce::Colour color) { textColor = color; repaint(); }
    
    // Icon support
    void setIcon(const juce::Path& iconPath) { icon = iconPath; hasIcon = true; repaint(); }
    void setIconSize(float size) { iconSize = size; repaint(); }
    void clearIcon() { hasIcon = false; repaint(); }
    
    // Animation
    void setAnimationEnabled(bool enabled) { animationEnabled = enabled; }
    
private:
    Style currentStyle;
    juce::Colour accentColor;
    juce::Colour backgroundColor;
    juce::Colour textColor;
    
    juce::Path icon;
    bool hasIcon = false;
    float iconSize = 16.0f;
    
    bool animationEnabled = true;
    float animationProgress = 0.0f;
    
    void paintPrimaryButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down);
    void paintSecondaryButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down);
    void paintToggleButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down);
    void paintIconButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down);
    void paintDangerButton(juce::Graphics& g, juce::Rectangle<int> bounds, bool highlighted, bool down);
    
    juce::Colour getButtonColor(bool highlighted, bool down) const;
    juce::Colour getTextColorForState(bool highlighted, bool down) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernButton)
};