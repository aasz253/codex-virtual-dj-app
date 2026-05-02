#include "DeckView.h"
#include "../Components/ModernButton.h"
#include "../Components/ModernSlider.h"
#include "../Components/WaveformView.h"
#include "../Components/VUMeter.h"
#include "../Components/JogWheel.h"
#include "../Model/Track.h"

DeckView::DeckView(DJController& controller, int deckIndex)
    : djController(controller),
      deckIndex(deckIndex),
      isPlaying(false),
      currentPosition(0.0)
{
    setupComponents();
    setupLayout();
    startTimerHz(30); // 30 FPS for UI updates
}

DeckView::~DeckView()
{
    stopTimer();
}

void DeckView::setupComponents()
{
    // Play/Pause button
    playButton = std::make_unique<ModernButton>("PLAY");
    playButton->setButtonStyle(ModernButton::Style::Primary);
    playButton->addListener(this);
    addAndMakeVisible(*playButton);
    
    // Stop button
    stopButton = std::make_unique<ModernButton>("STOP");
    stopButton->setButtonStyle(ModernButton::Style::Secondary);
    stopButton->addListener(this);
    addAndMakeVisible(*stopButton);
    
    // Cue button
    cueButton = std::make_unique<ModernButton>("CUE");
    cueButton->setButtonStyle(ModernButton::Style::Toggle);
    cueButton->addListener(this);
    addAndMakeVisible(*cueButton);
    
    // Load button
    loadButton = std::make_unique<ModernButton>("LOAD");
    loadButton->setButtonStyle(ModernButton::Style::Secondary);
    loadButton->addListener(this);
    addAndMakeVisible(*loadButton);
    
    // Sync button
    syncButton = std::make_unique<ModernButton>("SYNC");
    syncButton->setButtonStyle(ModernButton::Style::Toggle);
    syncButton->addListener(this);
    addAndMakeVisible(*syncButton);
    
    // Loop button
    loopButton = std::make_unique<ModernButton>("LOOP");
    loopButton->setButtonStyle(ModernButton::Style::Toggle);
    loopButton->addListener(this);
    addAndMakeVisible(*loopButton);
    
    // Gain slider
    gainSlider = std::make_unique<ModernSlider>();
    gainSlider->setSliderStyle(ModernSlider::Style::Vertical);
    gainSlider->setRange(0.0, 2.0, 0.01);
    gainSlider->setValue(1.0);
    gainSlider->addListener(this);
    addAndMakeVisible(*gainSlider);
    
    // Speed slider
    speedSlider = std::make_unique<ModernSlider>();
    speedSlider->setSliderStyle(ModernSlider::Style::Vertical);
    speedSlider->setRange(0.5, 2.0, 0.01);
    speedSlider->setValue(1.0);
    speedSlider->addListener(this);
    addAndMakeVisible(*speedSlider);
    
    // EQ sliders
    lowEQSlider = std::make_unique<ModernSlider>();
    lowEQSlider->setSliderStyle(ModernSlider::Style::EQ);
    lowEQSlider->setRange(-20.0, 20.0, 0.1);
    lowEQSlider->setValue(0.0);
    lowEQSlider->addListener(this);
    addAndMakeVisible(*lowEQSlider);
    
    midEQSlider = std::make_unique<ModernSlider>();
    midEQSlider->setSliderStyle(ModernSlider::Style::EQ);
    midEQSlider->setRange(-20.0, 20.0, 0.1);
    midEQSlider->setValue(0.0);
    midEQSlider->addListener(this);
    addAndMakeVisible(*midEQSlider);
    
    highEQSlider = std::make_unique<ModernSlider>();
    highEQSlider->setSliderStyle(ModernSlider::Style::EQ);
    highEQSlider->setRange(-20.0, 20.0, 0.1);
    highEQSlider->setValue(0.0);
    highEQSlider->addListener(this);
    addAndMakeVisible(*highEQSlider);
    
    // Waveform display
    waveformDisplay = std::make_unique<WaveformView>(djController.getFormatManager(), djController.getThumbnailCache());
    addAndMakeVisible(*waveformDisplay);
    
    // VU Meter
    vuMeter = std::make_unique<VUMeter>(VUMeter::Orientation::Vertical);
    addAndMakeVisible(*vuMeter);
    
    // Jog wheel
    jogWheel = std::make_unique<JogWheel>();
    jogWheel->addListener(this);
    addAndMakeVisible(*jogWheel);
    
    // Track info labels
    trackTitleLabel.setText("No Track Loaded", juce::dontSendNotification);
    trackTitleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    trackTitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffeeeeee));
    trackTitleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackTitleLabel);
    
    trackArtistLabel.setText("", juce::dontSendNotification);
    trackArtistLabel.setFont(juce::Font(12.0f));
    trackArtistLabel.setColour(juce::Label::textColourId, juce::Colour(0xffcccccc));
    trackArtistLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackArtistLabel);
    
    // Time labels
    trackTimeLabel.setText("00:00", juce::dontSendNotification);
    trackTimeLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    trackTimeLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00aaff));
    trackTimeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackTimeLabel);
    
    // BPM label
    trackBPMLabel.setText("120.0 BPM", juce::dontSendNotification);
    trackBPMLabel.setFont(juce::Font(12.0f));
    trackBPMLabel.setColour(juce::Label::textColourId, juce::Colour(0xffffff00));
    trackBPMLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trackBPMLabel);
}

void DeckView::setupLayout()
{
    // Set background color
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff1a1a1a));
}

void DeckView::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background gradient
    juce::ColourGradient gradient(juce::Colour(0xff2a2a2a), 0, 0,
                                 juce::Colour(0xff1a1a1a), 0, bounds.getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Drag-over highlight
    if (isDragOver)
    {
        g.setColour(juce::Colour(0x4000aaff));
        g.fillAll();
        g.setColour(juce::Colour(0xff00aaff));
        g.drawRect(bounds, 3);
    }
    
    // Border
    g.setColour(juce::Colour(0xff404040));
    g.drawRect(bounds, 1);
    
    // Deck number indicator
    auto deckIndicator = bounds.removeFromTop(30).reduced(10, 5);
    g.setColour(isDragOver ? juce::Colour(0xff00ffff) : juce::Colour(0xff00aaff));
    g.fillRoundedRectangle(deckIndicator.toFloat(), 3.0f);
    g.setColour(juce::Colour(0xffffffff));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("DECK " + juce::String(deckIndex + 1), deckIndicator, juce::Justification::centred);
}

void DeckView::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop(35); // Space for deck indicator
    
    // Top section - Track info
    auto topSection = bounds.removeFromTop(60);
    trackTitleLabel.setBounds(topSection.removeFromTop(25).reduced(10, 2));
    trackArtistLabel.setBounds(topSection.removeFromTop(20).reduced(10, 2));
    
    auto timeSection = topSection.reduced(10, 0);
    trackTimeLabel.setBounds(timeSection.removeFromLeft(timeSection.getWidth() / 2));
    trackBPMLabel.setBounds(timeSection);
    
    // Main content area
    auto mainArea = bounds.reduced(10);
    
    // Left side - Controls (made narrower to give more space to jog wheel)
    auto leftSide = mainArea.removeFromLeft(80);
    
    // Transport buttons
    auto buttonArea = leftSide.removeFromTop(120);
    auto buttonRow1 = buttonArea.removeFromTop(35);
    playButton->setBounds(buttonRow1.removeFromLeft(40).reduced(2));
    stopButton->setBounds(buttonRow1.reduced(2));
    
    auto buttonRow2 = buttonArea.removeFromTop(35);
    cueButton->setBounds(buttonRow2.removeFromLeft(40).reduced(2));
    loadButton->setBounds(buttonRow2.reduced(2));
    
    auto buttonRow3 = buttonArea.removeFromTop(35);
    syncButton->setBounds(buttonRow3.removeFromLeft(40).reduced(2));
    loopButton->setBounds(buttonRow3.reduced(2));
    
    leftSide.removeFromTop(10); // Spacing
    
    // Sliders (made smaller)
    auto sliderArea = leftSide.removeFromTop(120);
    auto gainArea = sliderArea.removeFromLeft(25);
    gainSlider->setBounds(gainArea.reduced(2));
    
    auto speedArea = sliderArea.removeFromLeft(25);
    speedSlider->setBounds(speedArea.reduced(2));
    
    // EQ section
    auto eqArea = sliderArea.reduced(5, 0);
    auto eqWidth = eqArea.getWidth() / 3;
    lowEQSlider->setBounds(eqArea.removeFromLeft(eqWidth).reduced(2));
    midEQSlider->setBounds(eqArea.removeFromLeft(eqWidth).reduced(2));
    highEQSlider->setBounds(eqArea.reduced(2));
    
    // Center - Waveform and jog wheel (give more space to jog wheel)
    auto centerArea = mainArea.removeFromLeft(mainArea.getWidth() - 60);
    
    // Waveform (made smaller to give more space to jog wheel)
    auto waveformArea = centerArea.removeFromTop(80);
    waveformDisplay->setBounds(waveformArea.reduced(5));
    
    centerArea.removeFromTop(10); // Spacing
    
    // Jog wheel (made larger)
    auto jogArea = centerArea.removeFromTop(320);
    auto jogSize = juce::jmin(jogArea.getWidth(), jogArea.getHeight());
    auto jogBounds = juce::Rectangle<int>(jogSize, jogSize).withCentre(jogArea.getCentre());
    jogWheel->setBounds(jogBounds);
    
    // Right side - VU Meter
    vuMeter->setBounds(mainArea.reduced(5));
}

void DeckView::buttonClicked(juce::Button* button)
{
    if (button == playButton.get())
    {
        djController.togglePlay(deckIndex);
    }
    else if (button == stopButton.get())
    {
        djController.stop(deckIndex);
    }
    else if (button == cueButton.get())
    {
        djController.cue(deckIndex);
    }
    else if (button == loadButton.get())
    {
        djController.loadTrack(deckIndex);
    }
    else if (button == syncButton.get())
    {
        djController.toggleSync(deckIndex);
    }
    else if (button == loopButton.get())
    {
        djController.toggleLoop(deckIndex);
    }
}

void DeckView::sliderValueChanged(juce::Slider* slider)
{
    if (slider == gainSlider.get())
    {
        djController.setDeckGain(deckIndex, slider->getValue());
    }
    else if (slider == speedSlider.get())
    {
        djController.setDeckSpeed(deckIndex, slider->getValue());
    }
    else if (slider == lowEQSlider.get())
    {
        // Update low EQ - need to get current mid and high values
        double low = slider->getValue();
        double mid = midEQSlider->getValue();
        double high = highEQSlider->getValue();
        djController.setDeckEQ(deckIndex, low, mid, high);
    }
    else if (slider == midEQSlider.get())
    {
        // Update mid EQ - need to get current low and high values
        double low = lowEQSlider->getValue();
        double mid = slider->getValue();
        double high = highEQSlider->getValue();
        djController.setDeckEQ(deckIndex, low, mid, high);
    }
    else if (slider == highEQSlider.get())
    {
        // Update high EQ - need to get current low and mid values
        double low = lowEQSlider->getValue();
        double mid = midEQSlider->getValue();
        double high = slider->getValue();
        djController.setDeckEQ(deckIndex, low, mid, high);
    }
}

void DeckView::timerCallback()
{
    // Update VU meter
    vuMeter->setLevel(rmsLevel, peakLevel);
    
    // Update time displays
    updateTimeDisplays();
    
    // Update play button state
    if (isPlaying && !isPaused)
    {
        playButton->setButtonText("PAUSE");
        playButton->setToggleState(true, juce::dontSendNotification);
    }
    else
    {
        playButton->setButtonText("PLAY");
        playButton->setToggleState(false, juce::dontSendNotification);
    }
}

void DeckView::updateTrackInfo(const juce::String& title, const juce::String& artist, double length)
{
    trackTitleLabel.setText(title.isEmpty() ? "No Track Loaded" : title, juce::dontSendNotification);
    trackArtistLabel.setText(artist, juce::dontSendNotification);
    trackLength = length;
    updateTimeDisplays();
}

void DeckView::updatePosition(double position)
{
    currentPosition = position;
    waveformDisplay->setPlaybackPosition(position);
    jogWheel->setRotation(static_cast<float>(position * juce::MathConstants<double>::twoPi));
}

void DeckView::updatePlayState(bool playing, bool paused)
{
    isPlaying = playing;
    isPaused = paused;
}

void DeckView::updateLevels(float rms, float peak)
{
    rmsLevel = rms;
    peakLevel = peak;
}

void DeckView::updateBPM(float bpm)
{
    trackBPMLabel.setText(juce::String(bpm, 1, false) + " BPM", juce::dontSendNotification);
}

void DeckView::loadTrack(const juce::File& file)
{
    Track track(file);
    waveformDisplay->loadTrack(track);
}

void DeckView::updateTimeDisplays()
{
    if (trackLength > 0.0)
    {
        int positionSeconds = static_cast<int>(currentPosition);
        int remainingSeconds = static_cast<int>(trackLength - currentPosition);
        
        auto formatTime = [](int seconds) {
            int mins = seconds / 60;
            int secs = seconds % 60;
            return juce::String::formatted("%02d:%02d", mins, secs);
        };
        
        trackTimeLabel.setText(formatTime(positionSeconds) + " / " + formatTime(remainingSeconds), juce::dontSendNotification);
    }
    else
    {
        trackTimeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    }
}



// JogWheel::Listener implementation
void DeckView::jogWheelMoved(JogWheel* wheel, float deltaAngle, bool isTouched)
{
    // Handle jog wheel movement - could adjust playback speed or position
    if (isTouched)
    {
        // Scratch mode - adjust position
        djController.adjustPosition(deckIndex, deltaAngle * 0.01);
    }
    else
    {
        // Pitch bend mode - adjust speed temporarily
        djController.adjustSpeed(deckIndex, deltaAngle * 0.1);
    }
}

void DeckView::jogWheelPressed(JogWheel* wheel, bool isPressed)
{
    // Handle jog wheel press if needed
}

void DeckView::jogWheelDoubleClicked(JogWheel* wheel)
{
    // Handle jog wheel double click (could reset to cue point)
    djController.cue(deckIndex);
}

// Drag-and-drop implementation
bool DeckView::isInterestedInFileDrag(const juce::StringArray& files)
{
    // Check if any file has an audio extension
    for (const auto& file : files)
    {
        juce::File f(file);
        if (f.hasFileExtension(".mp3;.wav;.flac;.aac;.ogg;.m4a;.mp4"))
            return true;
    }
    return false;
}

void DeckView::fileDragEnter(const juce::StringArray& files, int x, int y)
{
    isDragOver = true;
    repaint();
}

void DeckView::fileDragMove(const juce::StringArray& files, int x, int y)
{
    // Could add more visual feedback here
}

void DeckView::fileDragExit(const juce::StringArray& files)
{
    isDragOver = false;
    repaint();
}

void DeckView::filesDropped(const juce::StringArray& files, int x, int y)
{
    isDragOver = false;
    
    // Load the first valid audio file
    for (const auto& file : files)
    {
        juce::File audioFile(file);
        if (audioFile.hasFileExtension(".mp3;.wav;.flac;.aac;.ogg;.m4a;.mp4"))
        {
            Track track(audioFile);
            if (track.isValid())
            {
                djController.loadTrackToDeck(deckIndex, track);
                break; // Only load first valid file
            }
        }
    }
    
    repaint();
}