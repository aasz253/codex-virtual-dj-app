#include "VUMeter.h"

VUMeter::VUMeter(Orientation orientation)
    : orientation(orientation),
      greenColor(juce::Colour(0xff00ff00)),
      yellowColor(juce::Colour(0xffffff00)),
      redColor(juce::Colour(0xffff0000)),
      backgroundColor(juce::Colour(0xff1a1a1a))
{
    startTimerHz(60); // 60 FPS for smooth animation
}

VUMeter::~VUMeter()
{
    stopTimer();
}

void VUMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Fill background
    g.setColour(backgroundColor);
    g.fillRoundedRectangle(bounds.toFloat(), 2.0f);
    
    // Draw border
    g.setColour(juce::Colour(0xff333333));
    g.drawRoundedRectangle(bounds.toFloat(), 2.0f, 1.0f);
    
    // Draw meter based on orientation
    if (orientation == Orientation::Vertical)
        drawVerticalMeter(g, bounds.reduced(2));
    else
        drawHorizontalMeter(g, bounds.reduced(2));
    
    // Draw scale
    drawScale(g, bounds);
}

void VUMeter::resized()
{
    // Nothing specific to do here
}

void VUMeter::timerCallback()
{
    // Smooth animation for levels
    bool needsRepaint = false;
    
    // Animate RMS level
    if (std::abs(currentRMSLevel - targetRMSLevel) > 0.001f)
    {
        currentRMSLevel += (targetRMSLevel - currentRMSLevel) * (1.0f - smoothingFactor);
        needsRepaint = true;
    }
    
    // Animate peak level (faster response)
    if (targetPeakLevel > currentPeakLevel)
    {
        currentPeakLevel = targetPeakLevel; // Instant rise
        needsRepaint = true;
    }
    else if (currentPeakLevel > targetPeakLevel)
    {
        currentPeakLevel += (targetPeakLevel - currentPeakLevel) * 0.95f; // Slow fall
        needsRepaint = true;
    }
    
    // Handle peak hold
    if (showPeakHold)
    {
        if (currentPeakLevel > peakHoldLevel)
        {
            peakHoldLevel = currentPeakLevel;
            peakHoldTimer = 60; // Hold for 1 second at 60 FPS
        }
        else if (peakHoldTimer > 0)
        {
            peakHoldTimer--;
        }
        else
        {
            peakHoldLevel *= 0.99f; // Slow decay
            needsRepaint = true;
        }
    }
    
    if (needsRepaint)
        repaint();
}

void VUMeter::setLevel(float rmsLevel, float peakLevel)
{
    targetRMSLevel = juce::jlimit(0.0f, 1.0f, rmsLevel);
    targetPeakLevel = juce::jlimit(0.0f, 1.0f, peakLevel);
}

void VUMeter::setOrientation(Orientation newOrientation)
{
    if (orientation != newOrientation)
    {
        orientation = newOrientation;
        repaint();
    }
}

void VUMeter::setColors(juce::Colour green, juce::Colour yellow, juce::Colour red)
{
    greenColor = green;
    yellowColor = yellow;
    redColor = red;
    repaint();
}

void VUMeter::setRange(float minDb, float maxDb)
{
    this->minDb = minDb;
    this->maxDb = maxDb;
    repaint();
}

float VUMeter::levelToDb(float level) const
{
    if (level <= 0.0f)
        return minDb;
    return 20.0f * std::log10(level);
}

float VUMeter::dbToPosition(float db) const
{
    if (db <= minDb)
        return 0.0f;
    if (db >= maxDb)
        return 1.0f;
    return (db - minDb) / (maxDb - minDb);
}

juce::Colour VUMeter::getColorForLevel(float db) const
{
    if (db >= redZoneDb)
        return redColor;
    else if (db >= yellowZoneDb)
        return yellowColor;
    else
        return greenColor;
}

void VUMeter::drawVerticalMeter(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto meterBounds = bounds.reduced(4);
    int height = meterBounds.getHeight();
    
    // Draw RMS level
    if (showRMS && currentRMSLevel > 0.0f)
    {
        float rmsDb = levelToDb(currentRMSLevel);
        float rmsPosition = dbToPosition(rmsDb);
        int rmsHeight = static_cast<int>(rmsPosition * height);
        
        if (rmsHeight > 0)
        {
            auto rmsRect = juce::Rectangle<int>(meterBounds.getX(), 
                                              meterBounds.getBottom() - rmsHeight,
                                              meterBounds.getWidth() * 0.6f, 
                                              rmsHeight);
            
            // Create gradient for RMS
            juce::ColourGradient gradient(greenColor, 0, rmsRect.getBottom(),
                                        redColor, 0, rmsRect.getY(), false);
            gradient.addColour(0.7, yellowColor);
            g.setGradientFill(gradient);
            g.fillRoundedRectangle(rmsRect.toFloat(), 1.0f);
        }
    }
    
    // Draw peak level
    if (currentPeakLevel > 0.0f)
    {
        float peakDb = levelToDb(currentPeakLevel);
        float peakPosition = dbToPosition(peakDb);
        int peakHeight = static_cast<int>(peakPosition * height);
        
        if (peakHeight > 0)
        {
            auto peakRect = juce::Rectangle<int>(meterBounds.getX() + meterBounds.getWidth() * 0.7f,
                                               meterBounds.getBottom() - peakHeight,
                                               meterBounds.getWidth() * 0.3f,
                                               peakHeight);
            
            g.setColour(getColorForLevel(peakDb));
            g.fillRoundedRectangle(peakRect.toFloat(), 1.0f);
        }
    }
    
    // Draw peak hold
    if (showPeakHold && peakHoldLevel > 0.0f)
    {
        float holdDb = levelToDb(peakHoldLevel);
        float holdPosition = dbToPosition(holdDb);
        int holdY = meterBounds.getBottom() - static_cast<int>(holdPosition * height);
        
        g.setColour(getColorForLevel(holdDb));
        g.fillRect(meterBounds.getX(), holdY - 1, meterBounds.getWidth(), 2);
    }
}

void VUMeter::drawHorizontalMeter(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto meterBounds = bounds.reduced(4);
    int width = meterBounds.getWidth();
    
    // Draw RMS level
    if (showRMS && currentRMSLevel > 0.0f)
    {
        float rmsDb = levelToDb(currentRMSLevel);
        float rmsPosition = dbToPosition(rmsDb);
        int rmsWidth = static_cast<int>(rmsPosition * width);
        
        if (rmsWidth > 0)
        {
            auto rmsRect = juce::Rectangle<int>(meterBounds.getX(),
                                              meterBounds.getY(),
                                              rmsWidth,
                                              meterBounds.getHeight() * 0.6f);
            
            // Create gradient for RMS
            juce::ColourGradient gradient(greenColor, rmsRect.getX(), 0,
                                        redColor, rmsRect.getRight(), 0, false);
            gradient.addColour(0.7, yellowColor);
            g.setGradientFill(gradient);
            g.fillRoundedRectangle(rmsRect.toFloat(), 1.0f);
        }
    }
    
    // Draw peak level
    if (currentPeakLevel > 0.0f)
    {
        float peakDb = levelToDb(currentPeakLevel);
        float peakPosition = dbToPosition(peakDb);
        int peakWidth = static_cast<int>(peakPosition * width);
        
        if (peakWidth > 0)
        {
            auto peakRect = juce::Rectangle<int>(meterBounds.getX(),
                                               meterBounds.getY() + meterBounds.getHeight() * 0.7f,
                                               peakWidth,
                                               meterBounds.getHeight() * 0.3f);
            
            g.setColour(getColorForLevel(peakDb));
            g.fillRoundedRectangle(peakRect.toFloat(), 1.0f);
        }
    }
    
    // Draw peak hold
    if (showPeakHold && peakHoldLevel > 0.0f)
    {
        float holdDb = levelToDb(peakHoldLevel);
        float holdPosition = dbToPosition(holdDb);
        int holdX = meterBounds.getX() + static_cast<int>(holdPosition * width);
        
        g.setColour(getColorForLevel(holdDb));
        g.fillRect(holdX - 1, meterBounds.getY(), 2, meterBounds.getHeight());
    }
}

void VUMeter::drawScale(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setColour(juce::Colour(0xff666666));
    g.setFont(8.0f);
    
    if (orientation == Orientation::Vertical)
    {
        // Draw vertical scale marks
        std::vector<float> marks = {-60, -40, -20, -12, -6, 0};
        
        for (float db : marks)
        {
            if (db >= minDb && db <= maxDb)
            {
                float position = dbToPosition(db);
                int y = bounds.getBottom() - static_cast<int>(position * bounds.getHeight());
                
                g.drawHorizontalLine(y, bounds.getX(), bounds.getX() + 8);
                g.drawText(juce::String(static_cast<int>(db)), 
                          bounds.getX() + 10, y - 6, 20, 12, 
                          juce::Justification::centredLeft);
            }
        }
    }
    else
    {
        // Draw horizontal scale marks
        std::vector<float> marks = {-60, -40, -20, -12, -6, 0};
        
        for (float db : marks)
        {
            if (db >= minDb && db <= maxDb)
            {
                float position = dbToPosition(db);
                int x = bounds.getX() + static_cast<int>(position * bounds.getWidth());
                
                g.drawVerticalLine(x, bounds.getBottom() - 8, bounds.getBottom());
                g.drawText(juce::String(static_cast<int>(db)),
                          x - 10, bounds.getBottom() - 20, 20, 12,
                          juce::Justification::centred);
            }
        }
    }
}