#pragma once
#include <JuceHeader.h>

class ModernSlider : public juce::Slider
{
public:
    enum class Style
    {
        Vertical,
        Horizontal,
        Rotary,
        EQ
    };
    
    ModernSlider(Style sliderStyle = Style::Vertical);
    ~ModernSlider() override;
    
    void paint(juce::Graphics& g) override;
    
    void setSliderStyle(Style newStyle);
    void setAccentColor(juce::Colour color) { accentColor = color; repaint(); }
    void setBackgroundColor(juce::Colour color) { backgroundColor = color; repaint(); }
    void setTextColor(juce::Colour color) { textColor = color; repaint(); }
    
    // Custom label positioning
    void setLabelText(const juce::String& text) { labelText = text; repaint(); }
    void setShowLabel(bool show) { showLabel = show; repaint(); }
    void setShowValue(bool show) { showValue = show; repaint(); }
    
private:
    Style currentStyle;
    juce::Colour accentColor;
    juce::Colour backgroundColor;
    juce::Colour textColor;
    
    juce::String labelText;
    bool showLabel = true;
    bool showValue = true;
    
    void paintVerticalSlider(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintHorizontalSlider(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintRotarySlider(juce::Graphics& g, juce::Rectangle<int> bounds);
    void paintEQSlider(juce::Graphics& g, juce::Rectangle<int> bounds);
    
    juce::Rectangle<int> getSliderBounds() const;
    juce::Rectangle<int> getTextBounds() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernSlider)
};