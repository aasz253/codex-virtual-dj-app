#include <JuceHeader.h> // Include the JUCE framework header files
#include "DeckGUI.h" // Include the DeckGUI class header

DeckGUI::DeckGUI(DJAudioPlayer* _player,
                 AudioFormatManager & formatManagerToUse,
                 AudioThumbnailCache & cacheToUse) : player(_player),
playlistComponent(_player, formatManagerToUse, cacheToUse),meter([this]() -> float {
    // This lambda function is used to fetch the current RMS level for the meter
    return player->getCurrentRMSLevel();
}), disk(_player) // Initialize the disk component with the DJAudioPlayer
{
    // Set the style and text box of volume and speed sliders
    volSlider.setSliderStyle(juce::Slider::LinearVertical);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 15);
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 32, 15);
    // Make the disk, volume slider, and speed slider visible
    addAndMakeVisible(disk);
    setSize(400,400); // Set the size of the component
    
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    // Add listeners to the sliders to handle user interaction
    volSlider.addListener(this);
    speedSlider.addListener(this);
    
    // Set the range and step size of the volume and speed sliders
    volSlider.setRange(0.0, 1.0, 0.1);
    speedSlider.setRange(0.0, 1.0, 0.1);
    // Set the double-click return value for volume and speed sliders to default to 0
    volSlider.setDoubleClickReturnValue(true, 0);
    speedSlider.setDoubleClickReturnValue(true, 0);
    // Make the meter component visible
    addAndMakeVisible(meter);
    startTimerHz(30); // Start a timer to update the GUI at 30Hz
}

DeckGUI::~DeckGUI()
{
    // Cleanup: stop the timer and detach the custom LookAndFeel
    stopTimer();
    volSlider.setLookAndFeel(nullptr);
    speedSlider.setLookAndFeel(nullptr);
}

void DeckGUI::paint (juce::Graphics& g)
{
    // Draw the component background, outline, and text
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (juce::Colours::lightgrey);
    g.drawRect (getLocalBounds(), 1); // Draw an outline around the component
    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("DeckGUI", getLocalBounds(),
                juce::Justification::centred, true); // Draw some placeholder text
}

void DeckGUI::resized()
{
    // Layout the sliders, disk, and meter within the component
    volSlider.setBounds(5, 100, 72, 164);
    speedSlider.setBounds(50, 100, 72, 164);
    disk.setBounds(getLocalBounds().reduced(80));
    meter.setBounds(getLocalBounds().reduced(20).removeFromRight(30));
    auto meterBounds = getLocalBounds().reduced(10).removeFromRight(30);
    meter.setBounds(meterBounds);
}

void DeckGUI::buttonClicked (Button* button){}// Empty implementation for button click events
// Adjust the audio player's gain or speed based on slider changes
void DeckGUI::sliderValueChanged (Slider *slider)
{
    if(slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }
    if(slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
}

void DeckGUI::updateDiskRotation() {
    // Update the disk's rotation angle based on the current play position
    if (player->isPlaying()) {
        auto playPosition = player->getCurrentPosition();
        auto playLength = player->getLengthInSeconds();

        if (playLength > 0) {
            auto targetAngle = juce::jmap<float>(playPosition, 0.0f, playLength, 0.0f, 2.0f * juce::MathConstants<float>::pi);
            disk.setTargetAngle(targetAngle);
        }
    }
}

void DeckGUI::timerCallback() {
    // Called periodically by the timer to repaint the meter and update the DJDisk
    meter.repaint();
    updateDiskRotation();
}

