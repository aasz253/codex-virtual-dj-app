#include "DJDisk.h"

DJDisk::DJDisk(DJAudioPlayer* player) : player(player)
{
    // Set this component to be a focus container for keyboard events
    setFocusContainerType(juce::Component::FocusContainerType::keyboardFocusContainer);
    setWantsKeyboardFocus(true);// Indicate that this component wants to receive keyboard focus
    
    startTimerHz(30); // Update at 30 frames per second
    currentAngle = 0.0f; // Initialize current angle of the disk to 0
    targetAngle = 0.0f; // Initialize target angle of the disk to 0
}

DJDisk::~DJDisk()
{
}

void DJDisk::setTargetAngle(float angle)
{
    targetAngle = angle; // Set the new target angle
}

void DJDisk::timerCallback()
{
    // Smoothly update the current angle towards the target angle
    const float smoothingFactor = isDragging ? 0.1f : 0.05f; // Adjust this for smoother or faster rotation
    currentAngle += (targetAngle - currentAngle) * smoothingFactor;

    repaint(); // Repaint to show the updated angle
}

bool DJDisk::keyPressed(const juce::KeyPress& key)
{
    // Calculate the proportion of the disk's full rotation that corresponds to a 5-second jump
    auto totalRotation = 2.0f * juce::MathConstants<float>::pi; // Full rotation in radians
    auto playLength = player->getLengthInSeconds();// Total length of the track in seconds
    // Proportion of the disk rotation corresponding to a 5-second
    auto fiveSecondProportion = (2.0f / playLength) * totalRotation;

    if (key == juce::KeyPress::leftKey) {
        // Rotate left for a 5-second jump
        targetAngle -= fiveSecondProportion;
    }
    else if (key == juce::KeyPress::rightKey) {
        // Rotate right for a 5-second jump
        targetAngle += fiveSecondProportion;
    }

    // Normalize the target angle to ensure it remains within the 0 to 2*pi range
    targetAngle = fmod(targetAngle + totalRotation, totalRotation);

    return true; // Indicate that the key press was handled
}

void DJDisk::updatePlaybackPosition()
{
    auto totalRotation = 2.0f * juce::MathConstants<float>::pi; // Full rotation in radians
    auto playLength = player->getLengthInSeconds(); // Obtain the total length of the track
    auto newPosition = (currentAngle / totalRotation) * playLength;// Calculate the new position in seconds
    player->setPosition(newPosition);// Set the new playback position
}


void DJDisk::mouseDown(const juce::MouseEvent& event)
{
    // Remember the start position for the drag operation
    dragStartAngle = currentAngle;
    isDragging = true;
    grabKeyboardFocus(); // Grab focus on mouse click
}

void DJDisk::mouseDrag(const juce::MouseEvent& event)
{
    if (!isDragging) return;// Only proceed if a drag operation is in progress

    auto center = getLocalBounds().toFloat().getCentre();// Get the center point of the disk
    // Calculate the change in angle based on the drag
    auto angleDelta = atan2(event.position.y - center.y, event.position.x - center.x) - atan2(dragStartPos.y - center.y, dragStartPos.x - center.x);
    setTargetAngle(dragStartAngle + angleDelta);// Update the target angle based on the drag
}

void DJDisk::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;// Indicate that the drag operation has ended
    // Optionally, update the playback position here if needed
    auto totalRotation = 2.0 * juce::MathConstants<float>::pi; // Full rotation
    auto playLength = player->getLengthInSeconds(); // Obtain the total length of the track
    auto newPosition = (currentAngle / totalRotation) * playLength;
    player->setPosition(newPosition);
}

void DJDisk::paint(juce::Graphics& g)
{
    // Draw the disk and its rotation indicator
    auto bounds = getLocalBounds().toFloat();// Get the bounds of the component
    auto center = bounds.getCentre();// Calculate the center of the component
    auto diameter = juce::jmin(bounds.getWidth(), bounds.getHeight()) - 10; // Ensure some padding
    auto diskBounds = juce::Rectangle<float>(diameter, diameter).withCentre(center);// Define the bounds of the disk

    // Calculate the smoothed angle
    currentAngle += (targetAngle - currentAngle) * 0.1f;

    // Draw the disk
    g.setColour(juce::Colours::lightgrey);
    g.fillEllipse(diskBounds);

    // Draw the indicator
    g.setColour(juce::Colours::darkgrey);
    juce::Path p;
    p.startNewSubPath(center.x, center.y);
    p.lineTo(center.x, diskBounds.getY());
    p.applyTransform(juce::AffineTransform::rotation(currentAngle, center.x, center.y));
    g.strokePath(p, juce::PathStrokeType(8.0f));

    // Draw a spindle hole
    g.setColour(juce::Colours::black);
    g.fillEllipse(center.x - 10, center.y - 10, 20, 20);
}

