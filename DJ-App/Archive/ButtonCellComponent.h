#pragma once
#include <JuceHeader.h>
#include <functional>

class ButtonCellComponent  : public juce::Component,
                             public Button::Listener
{
public:
    ButtonCellComponent();
    ~ButtonCellComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void paintRowBackground (Graphics & g,
                             int rowNumber,
                             int width,
                             int height,
                             bool rowIsSelected);
    
    void buttonClicked(Button* button) override;
    
    // Inside ButtonCellComponent class
    using ButtonClickedCallback = std::function<void(const juce::String&)>;
    
    void setOnButtonClicked(ButtonClickedCallback newCallback);
    void setRowNumber(int newRowNumber);
    int getSelectedRow();
    
private:
    
    ButtonClickedCallback onButtonClicked;
    
    int rowNumber;
    
    juce::TextButton button1{"PLAY"};
    juce::TextButton button2{"PAUSE"};
    juce::TextButton button3{"DELETE"};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonCellComponent)
};
    

    

    

    
    

    
   
    
    


