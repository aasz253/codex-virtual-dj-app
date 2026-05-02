#include "MixerView.h"

MixerView::MixerView()
{
    setupComponents();
    startTimerHz(30); // 30 FPS for UI updates
}

MixerView::~MixerView()
{
    stopTimer();
}

void MixerView::setupComponents()
{
    // Crossfader
    crossfaderSlider = std::make_unique<ModernSlider>();
    crossfaderSlider->setSliderStyle(ModernSlider::Style::Horizontal);
    crossfaderSlider->setRange(-1.0, 1.0, 0.01);
    crossfaderSlider->setValue(0.0);
    crossfaderSlider->addListener(this);
    addAndMakeVisible(*crossfaderSlider);
    
    crossfaderLabel = std::make_unique<juce::Label>("crossfader", "CROSSFADER");
    crossfaderLabel->setFont(juce::Font(12.0f, juce::Font::bold));
    crossfaderLabel->setColour(juce::Label::textColourId, juce::Colour(0xffcccccc));
    crossfaderLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*crossfaderLabel);
    
    // Master section
    masterGainSlider = std::make_unique<ModernSlider>();
    masterGainSlider->setSliderStyle(ModernSlider::Style::Vertical);
    masterGainSlider->setRange(0.0, 2.0, 0.01);
    masterGainSlider->setValue(0.8);
    masterGainSlider->addListener(this);
    addAndMakeVisible(*masterGainSlider);
    
    masterFilterSlider = std::make_unique<ModernSlider>();
    masterFilterSlider->setSliderStyle(ModernSlider::Style::Rotary);
    masterFilterSlider->setRange(-1.0, 1.0, 0.01);
    masterFilterSlider->setValue(0.0);
    masterFilterSlider->addListener(this);
    addAndMakeVisible(*masterFilterSlider);
    
    masterVUMeter = std::make_unique<VUMeter>(VUMeter::Orientation::Vertical);
    masterVUMeter->setColors(juce::Colour(0xff00ff00), juce::Colour(0xffffff00), juce::Colour(0xffff0000));
    addAndMakeVisible(*masterVUMeter);
    
    masterLabel = std::make_unique<juce::Label>("master", "MASTER");
    masterLabel->setFont(juce::Font(12.0f, juce::Font::bold));
    masterLabel->setColour(juce::Label::textColourId, juce::Colour(0xffeeeeee));
    masterLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*masterLabel);
    
    // Cue section
    cueGainSlider = std::make_unique<ModernSlider>();
    cueGainSlider->setSliderStyle(ModernSlider::Style::Vertical);
    cueGainSlider->setRange(0.0, 1.0, 0.01);
    cueGainSlider->setValue(0.5);
    cueGainSlider->addListener(this);
    addAndMakeVisible(*cueGainSlider);
    
    cueVUMeter = std::make_unique<VUMeter>(VUMeter::Orientation::Vertical);
    cueVUMeter->setColors(juce::Colour(0xff0088ff), juce::Colour(0xff00aaff), juce::Colour(0xff0066cc));
    addAndMakeVisible(*cueVUMeter);
    
    cueLabel = std::make_unique<juce::Label>("cue", "CUE");
    cueLabel->setFont(juce::Font(12.0f, juce::Font::bold));
    cueLabel->setColour(juce::Label::textColourId, juce::Colour(0xff00aaff));
    cueLabel->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*cueLabel);
    
    // Control buttons
    recordButton = std::make_unique<ModernButton>("REC");
    recordButton->setButtonStyle(ModernButton::Style::Danger);
    recordButton->addListener(this);
    addAndMakeVisible(*recordButton);
    
    autoMixButton = std::make_unique<ModernButton>("AUTO");
    autoMixButton->setButtonStyle(ModernButton::Style::Toggle);
    autoMixButton->addListener(this);
    addAndMakeVisible(*autoMixButton);
    
    beatSyncButton = std::make_unique<ModernButton>("SYNC");
    beatSyncButton->setButtonStyle(ModernButton::Style::Primary);
    beatSyncButton->addListener(this);
    addAndMakeVisible(*beatSyncButton);
}

void MixerView::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Background gradient
    juce::ColourGradient gradient(juce::Colour(0xff2a2a2a), 0, 0,
                                 juce::Colour(0xff1a1a1a), 0, bounds.getHeight(), false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Border
    g.setColour(juce::Colour(0xff404040));
    g.drawRect(bounds, 1);
    
    // Section dividers
    g.setColour(juce::Colour(0xff333333));
    
    // Vertical dividers
    int sectionWidth = bounds.getWidth() / 3;
    g.drawVerticalLine(sectionWidth, 10, bounds.getHeight() - 10);
    g.drawVerticalLine(sectionWidth * 2, 10, bounds.getHeight() - 10);
    
    // Crossfader section background
    auto crossfaderArea = bounds.removeFromBottom(80).reduced(10);
    g.setColour(juce::Colour(0xff252525));
    g.fillRoundedRectangle(crossfaderArea.toFloat(), 5.0f);
    g.setColour(juce::Colour(0xff404040));
    g.drawRoundedRectangle(crossfaderArea.toFloat(), 5.0f, 1.0f);
    
    // Recording indicator
    if (isRecording)
    {
        auto recIndicator = bounds.removeFromTop(20).removeFromRight(60).reduced(2);
        g.setColour(juce::Colour(0xffff0000));
        g.fillRoundedRectangle(recIndicator.toFloat(), 3.0f);
        g.setColour(juce::Colour(0xffffffff));
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        g.drawText("● REC", recIndicator, juce::Justification::centred);
    }
}

void MixerView::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // Reserve space for recording indicator
    if (isRecording)
        bounds.removeFromTop(25);
    
    // Crossfader section at bottom
    auto crossfaderArea = bounds.removeFromBottom(60);
    crossfaderLabel->setBounds(crossfaderArea.removeFromTop(20));
    crossfaderSlider->setBounds(crossfaderArea.reduced(20, 10));
    
    bounds.removeFromBottom(10); // Spacing
    
    // Control buttons section
    auto buttonArea = bounds.removeFromBottom(40);
    int buttonWidth = buttonArea.getWidth() / 3;
    recordButton->setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    autoMixButton->setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    beatSyncButton->setBounds(buttonArea.reduced(5));
    
    bounds.removeFromBottom(10); // Spacing
    
    // Main mixer sections
    int sectionWidth = bounds.getWidth() / 3;
    
    // Cue section (left)
    auto cueSection = bounds.removeFromLeft(sectionWidth).reduced(5);
    cueLabel->setBounds(cueSection.removeFromTop(20));
    
    auto cueControls = cueSection.removeFromTop(cueSection.getHeight() - 20);
    cueGainSlider->setBounds(cueControls.removeFromLeft(30).reduced(2));
    cueVUMeter->setBounds(cueControls.reduced(5));
    
    // Master section (right)
    auto masterSection = bounds.removeFromRight(sectionWidth).reduced(5);
    masterLabel->setBounds(masterSection.removeFromTop(20));
    
    auto masterControls = masterSection.removeFromTop(masterSection.getHeight() - 20);
    auto masterSliders = masterControls.removeFromLeft(60);
    
    masterGainSlider->setBounds(masterSliders.removeFromLeft(30).reduced(2));
    masterFilterSlider->setBounds(masterSliders.removeFromTop(60).reduced(2));
    
    masterVUMeter->setBounds(masterControls.reduced(5));
}

void MixerView::sliderValueChanged(juce::Slider* slider)
{
    if (slider == crossfaderSlider.get())
    {
        if (onCrossfaderChanged)
            onCrossfaderChanged(static_cast<float>(slider->getValue()));
    }
    else if (slider == masterGainSlider.get())
    {
        if (onMasterGainChanged)
            onMasterGainChanged(static_cast<float>(slider->getValue()));
    }
    else if (slider == cueGainSlider.get())
    {
        if (onCueGainChanged)
            onCueGainChanged(static_cast<float>(slider->getValue()));
    }
    else if (slider == masterFilterSlider.get())
    {
        if (onMasterFilterChanged)
            onMasterFilterChanged(static_cast<float>(slider->getValue()));
    }
}

void MixerView::buttonClicked(juce::Button* button)
{
    if (button == recordButton.get())
    {
        if (onRecordingToggled)
            onRecordingToggled(!isRecording);
    }
    else if (button == autoMixButton.get())
    {
        if (onAutoMixToggled)
            onAutoMixToggled(button->getToggleState());
    }
    else if (button == beatSyncButton.get())
    {
        if (onBeatSyncPressed)
            onBeatSyncPressed();
    }
}

void MixerView::timerCallback()
{
    // Update VU meters
    masterVUMeter->setLevel(masterRMSLevel, masterPeakLevel);
    cueVUMeter->setLevel(cueRMSLevel, cuePeakLevel);
    
    // Update recording button state
    if (isRecording)
    {
        recordButton->setToggleState(true, juce::dontSendNotification);
        recordButton->setButtonText("STOP");
    }
    else
    {
        recordButton->setToggleState(false, juce::dontSendNotification);
        recordButton->setButtonText("REC");
    }
}

void MixerView::updateMasterLevels(float rms, float peak)
{
    masterRMSLevel = rms;
    masterPeakLevel = peak;
}

void MixerView::updateCueLevels(float rms, float peak)
{
    cueRMSLevel = rms;
    cuePeakLevel = peak;
}

void MixerView::updateRecordingState(bool recording)
{
    if (isRecording != recording)
    {
        isRecording = recording;
        resized(); // Trigger layout update for recording indicator
        repaint();
    }
}