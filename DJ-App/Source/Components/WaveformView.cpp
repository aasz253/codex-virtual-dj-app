#include "WaveformView.h"

WaveformView::WaveformView(juce::AudioFormatManager& fm, juce::AudioThumbnailCache& cache)
    : formatManager(fm), audioThumbnail(512, formatManager, cache),
      waveformColor(juce::Colour(0xff00d4ff)),
      backgroundColor(juce::Colour(0xff1a1a1a)),
      positionColor(juce::Colour(0xffff6b35)),
      cueColor(juce::Colour(0xff00ff88)),
      loopColor(juce::Colour(0xffff88ff))
{
    audioThumbnail.addChangeListener(this);
    startTimerHz(30); // 30 FPS for smooth position updates
}

WaveformView::~WaveformView()
{
    audioThumbnail.removeChangeListener(this);
}

void WaveformView::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background
    g.fillAll(backgroundColor);
    
    // Border
    g.setColour(backgroundColor.brighter(0.2f));
    g.drawRect(bounds, 1);
    
    if (audioThumbnail.getNumChannels() == 0)
    {
        // No audio loaded
        g.setColour(waveformColor.withAlpha(0.3f));
        g.setFont(14.0f);
        g.drawText("No audio loaded", bounds, juce::Justification::centred);
        return;
    }
    
    auto waveformBounds = bounds.reduced(2);
    auto timeScaleBounds = waveformBounds.removeFromBottom(20);
    
    // Draw waveform
    drawWaveform(g, waveformBounds);
    
    // Draw loop region (behind other elements)
    drawLoopRegion(g, waveformBounds);
    
    // Draw cue point
    drawCuePoint(g, waveformBounds);
    
    // Draw position marker
    drawPositionMarker(g, waveformBounds);
    
    // Draw time scale
    drawTimeScale(g, timeScaleBounds);
}

void WaveformView::resized()
{
    updateViewForZoom();
}

void WaveformView::mouseDown(const juce::MouseEvent& event)
{
    if (audioThumbnail.getNumChannels() == 0)
        return;
    
    auto clickTime = pixelToTime(event.x);
    
    if (event.mods.isRightButtonDown())
    {
        // Right click sets cue point
        setCuePoint(clickTime);
        if (onCuePointSet)
            onCuePointSet(clickTime);
    }
    else if (event.mods.isShiftDown())
    {
        // Shift+click sets loop points
        if (loopStart < 0)
        {
            setLoopStart(clickTime);
        }
        else if (loopEnd < 0)
        {
            setLoopEnd(clickTime);
            if (onLoopSet)
                onLoopSet(loopStart, loopEnd);
        }
        else
        {
            // Clear loop and start new one
            clearLoop();
            setLoopStart(clickTime);
        }
    }
    else
    {
        // Normal click sets position
        setPosition(clickTime / audioThumbnail.getTotalLength());
        isDragging = true;
        
        if (onPositionChanged)
            onPositionChanged(currentPosition);
    }
}

void WaveformView::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging && audioThumbnail.getNumChannels() > 0)
    {
        auto dragTime = pixelToTime(event.x);
        setPosition(dragTime / audioThumbnail.getTotalLength());
        
        if (onPositionChanged)
            onPositionChanged(currentPosition);
    }
}

void WaveformView::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &audioThumbnail)
    {
        repaint();
    }
}

void WaveformView::timerCallback()
{
    // Smooth position updates
    repaint();
}

void WaveformView::loadTrack(const Track& track)
{
    audioThumbnail.setSource(new juce::FileInputSource(track.getFile()));
    currentPosition = 0.0;
    playbackPosition = 0.0;
    clearCuePoint();
    clearLoop();
    repaint();
}

void WaveformView::setPosition(double position)
{
    currentPosition = juce::jlimit(0.0, 1.0, position);
    repaint();
}

void WaveformView::setZoom(double newZoomLevel)
{
    zoomLevel = juce::jmax(1.0, newZoomLevel);
    updateViewForZoom();
    repaint();
}

void WaveformView::setPlaybackPosition(double position)
{
    playbackPosition = juce::jlimit(0.0, 1.0, position);
    repaint();
}

void WaveformView::setCuePoint(double position)
{
    cuePoint = juce::jlimit(0.0, 1.0, position);
    repaint();
}

void WaveformView::clearCuePoint()
{
    cuePoint = -1.0;
    repaint();
}

void WaveformView::setLoopStart(double position)
{
    loopStart = juce::jlimit(0.0, 1.0, position);
    repaint();
}

void WaveformView::setLoopEnd(double position)
{
    loopEnd = juce::jlimit(loopStart, 1.0, position);
    repaint();
}

void WaveformView::clearLoop()
{
    loopStart = -1.0;
    loopEnd = -1.0;
    repaint();
}

double WaveformView::pixelToTime(int pixel) const
{
    auto totalLength = audioThumbnail.getTotalLength();
    auto visibleLength = totalLength / zoomLevel;
    auto timePerPixel = visibleLength / getWidth();
    return viewStart + (pixel * timePerPixel);
}

int WaveformView::timeToPixel(double time) const
{
    auto totalLength = audioThumbnail.getTotalLength();
    auto visibleLength = totalLength / zoomLevel;
    auto timePerPixel = visibleLength / getWidth();
    return static_cast<int>((time - viewStart) / timePerPixel);
}

void WaveformView::updateViewForZoom()
{
    auto totalLength = audioThumbnail.getTotalLength();
    auto visibleLength = totalLength / zoomLevel;
    
    // Center view around current position
    viewStart = (currentPosition * totalLength) - (visibleLength * 0.5);
    viewStart = juce::jlimit(0.0, totalLength - visibleLength, viewStart);
}

void WaveformView::drawWaveform(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (audioThumbnail.getNumChannels() == 0)
        return;
    
    auto totalLength = audioThumbnail.getTotalLength();
    auto visibleLength = totalLength / zoomLevel;
    auto endTime = viewStart + visibleLength;
    
    // Draw waveform with gradient
    juce::ColourGradient gradient(waveformColor.withAlpha(0.8f), 0, bounds.getY(),
                                 waveformColor.withAlpha(0.3f), 0, bounds.getBottom(), false);
    g.setGradientFill(gradient);
    
    audioThumbnail.drawChannels(g, bounds, viewStart, endTime, 1.0f);
    
    // Draw center line
    g.setColour(backgroundColor.brighter(0.3f));
    auto centerY = bounds.getCentreY();
    g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, 1.0f);
}

void WaveformView::drawPositionMarker(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (audioThumbnail.getNumChannels() == 0)
        return;
    
    auto totalLength = audioThumbnail.getTotalLength();
    auto currentTime = playbackPosition * totalLength;
    auto pixelPos = timeToPixel(currentTime);
    
    if (pixelPos >= 0 && pixelPos < getWidth())
    {
        // Position line
        g.setColour(positionColor);
        g.drawLine(pixelPos, bounds.getY(), pixelPos, bounds.getBottom(), 2.0f);
        
        // Position marker at top
        juce::Path marker;
        marker.addTriangle(pixelPos - 5, bounds.getY(), pixelPos + 5, bounds.getY(), pixelPos, bounds.getY() + 10);
        g.fillPath(marker);
    }
}

void WaveformView::drawCuePoint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (cuePoint < 0 || audioThumbnail.getNumChannels() == 0)
        return;
    
    auto totalLength = audioThumbnail.getTotalLength();
    auto cueTime = cuePoint * totalLength;
    auto pixelPos = timeToPixel(cueTime);
    
    if (pixelPos >= 0 && pixelPos < getWidth())
    {
        // Cue line
        g.setColour(cueColor);
        g.drawLine(pixelPos, bounds.getY(), pixelPos, bounds.getBottom(), 1.5f);
        
        // Cue marker
        g.fillEllipse(pixelPos - 3, bounds.getY() + 2, 6, 6);
        
        // Label
        g.setFont(10.0f);
        g.drawText("CUE", pixelPos - 15, bounds.getY() + 10, 30, 12, juce::Justification::centred);
    }
}

void WaveformView::drawLoopRegion(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (loopStart < 0 || loopEnd < 0 || audioThumbnail.getNumChannels() == 0)
        return;
    
    auto totalLength = audioThumbnail.getTotalLength();
    auto startTime = loopStart * totalLength;
    auto endTime = loopEnd * totalLength;
    auto startPixel = timeToPixel(startTime);
    auto endPixel = timeToPixel(endTime);
    
    if (endPixel > 0 && startPixel < getWidth())
    {
        auto loopBounds = juce::Rectangle<int>(juce::jmax(0, startPixel), bounds.getY(),
                                              juce::jmin(getWidth(), endPixel) - juce::jmax(0, startPixel),
                                              bounds.getHeight());
        
        // Loop region background
        g.setColour(loopColor.withAlpha(0.2f));
        g.fillRect(loopBounds);
        
        // Loop boundaries
        g.setColour(loopColor);
        if (startPixel >= 0 && startPixel < getWidth())
            g.drawLine(startPixel, bounds.getY(), startPixel, bounds.getBottom(), 2.0f);
        if (endPixel >= 0 && endPixel < getWidth())
            g.drawLine(endPixel, bounds.getY(), endPixel, bounds.getBottom(), 2.0f);
        
        // Loop label
        g.setFont(10.0f);
        auto labelBounds = loopBounds.removeFromTop(15);
        g.drawText("LOOP", labelBounds, juce::Justification::centred);
    }
}

void WaveformView::drawTimeScale(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (audioThumbnail.getNumChannels() == 0)
        return;
    
    g.setColour(waveformColor.withAlpha(0.6f));
    g.setFont(9.0f);
    
    auto totalLength = audioThumbnail.getTotalLength();
    auto visibleLength = totalLength / zoomLevel;
    
    // Calculate appropriate time intervals
    auto interval = visibleLength / 10.0; // Aim for ~10 markers
    if (interval < 1.0)
        interval = 0.1;
    else if (interval < 5.0)
        interval = 1.0;
    else if (interval < 10.0)
        interval = 5.0;
    else if (interval < 30.0)
        interval = 10.0;
    else if (interval < 60.0)
        interval = 30.0;
    else
        interval = 60.0;
    
    auto startTime = std::floor(viewStart / interval) * interval;
    
    for (auto time = startTime; time <= viewStart + visibleLength; time += interval)
    {
        auto pixelPos = timeToPixel(time);
        if (pixelPos >= 0 && pixelPos < getWidth())
        {
            // Time marker
            g.drawLine(pixelPos, bounds.getY(), pixelPos, bounds.getY() + 5, 1.0f);
            
            // Time label
            auto minutes = static_cast<int>(time) / 60;
            auto seconds = static_cast<int>(time) % 60;
            auto timeText = juce::String::formatted("%d:%02d", minutes, seconds);
            g.drawText(timeText, pixelPos - 20, bounds.getY() + 6, 40, 12, juce::Justification::centred);
        }
    }
}