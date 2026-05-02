#include "VerticalGradientMeter.h"

Gui::VerticalGradientMeter::VerticalGradientMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
{
    startTimerHz(24);// Start a timer to trigger repaints at 24 frames per second
    // Create a simple grill pattern programmatically instead of loading from binary data
    grill = Image(Image::ARGB, 100, 100, true);
    Graphics grillGraphics(grill);
    grillGraphics.setColour(Colours::black.withAlpha(0.3f));
    for (int i = 0; i < 100; i += 4)
    {
        grillGraphics.drawHorizontalLine(i, 0.0f, 100.0f);
    }
}

Gui::VerticalGradientMeter::~VerticalGradientMeter()
{
}

void Gui::VerticalGradientMeter::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(3.0f);
    auto level = valueSupplier(); // Fetch the current level

    g.setColour(Colours::black);// Set the fill colour to black
    g.fillRect(bounds);// Fill the component background

    float scaleFactor = 3.2; //multiplier to control the intensity
    // Ensure level is not exactly 0 to avoid log(0)
    float adjustedLevel = level > 0.0f ? std::log10(level * 9.0f + 1.0f)* scaleFactor : 0.0f;
    
    // Map the level value to the height of the component
    auto fillHeight = jmap(adjustedLevel, 0.0f, 1.0f, 0.0f, static_cast<float>(bounds.getHeight()));
    g.setGradientFill(gradient);// Set the fill to use the pre-defined gradient
    g.fillRect(bounds.removeFromBottom(fillHeight));// Fill the bottom part of the component according to the level
}


void Gui::VerticalGradientMeter::resized()
{
    const auto bounds = getLocalBounds().toFloat();// Get the bounds of the component
    gradient = ColourGradient{ Colours::green, bounds.getBottomLeft(), Colours::red, bounds.getTopLeft(), false };// Create a vertical gradient from green at the bottom to red at the top
    gradient.addColour(0.5, Colours::yellow);// Add yellow at the midpoint of the gradient
}

void Gui::VerticalGradientMeter::paintOverChildren(::juce::Graphics& g)
{
    g.drawImage(grill, getLocalBounds().toFloat());// Draw the grill image over the component
}

void Gui::VerticalGradientMeter::timerCallback() 
{
    repaint(); // Trigger a repaint of the component at each timer callback
}
