
#include <JuceHeader.h>
#include "ButtonCellComponent.h"

ButtonCellComponent::ButtonCellComponent() : rowNumber(-1)// Constructor initializing rowNumber with -1
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(button1);
    addAndMakeVisible(button2);
    addAndMakeVisible(button3);
    //Set texts in buttons
    button1.setButtonText("PLAY");
    button2.setButtonText("PAUSE");
    button3.setButtonText("DELETE");
    //listeners
    button1.addListener(this);
    button2.addListener(this);
    button3.addListener(this);
    //set IDs
    button1.setComponentID("PLAY");
    button2.setComponentID("PAUSE");
    button3.setComponentID("DELETE");
}

ButtonCellComponent::~ButtonCellComponent()
{
    // Destructor
}

void ButtonCellComponent::paint (juce::Graphics& g)
{
    // Set the color and font for drawing. This function currently does no drawing.
    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
}

void ButtonCellComponent::resized()
{
    // Arrange the buttons within the component when it's resized
    auto bounds = getLocalBounds();
    auto buttonSize = juce::jmin(bounds.getWidth(), bounds.getHeight()/2);
    // Set the bounds for each button
    button1.setBounds(0, (bounds.getHeight() - buttonSize) / 11, buttonSize*6, buttonSize*2);
    button2.setBounds(buttonSize*6, (bounds.getHeight() - buttonSize) / 11, buttonSize*6, buttonSize*2);
    button3.setBounds(buttonSize*12, (bounds.getHeight() - buttonSize) / 11, buttonSize*6, buttonSize*2);
}

void ButtonCellComponent::paintRowBackground (Graphics & g,
                         int rowNumber,
                         int width,
                         int height,
                         bool rowIsSelected)
{
    // Paint the background of a table row
    if(rowIsSelected)
    {
        g.fillAll(Colours::orange);// If the row is selected, fill the background with orange
    }
};


void ButtonCellComponent::buttonClicked(Button* button)
{
    // Handle button click events
    if (onButtonClicked) {
        // If the callback is set, call it with the ID of the clicked button
        onButtonClicked(button->getComponentID());
    }
}

void ButtonCellComponent::setOnButtonClicked(ButtonClickedCallback newCallback) {
    // Set the callback function to be called when a button is clicked
    onButtonClicked = std::move(newCallback);
}

void ButtonCellComponent::setRowNumber(int newRowNumber) {
    // Set the row number for this component
    rowNumber = newRowNumber;
}

int ButtonCellComponent::getSelectedRow(){
    return rowNumber;// Return the current row number
}

