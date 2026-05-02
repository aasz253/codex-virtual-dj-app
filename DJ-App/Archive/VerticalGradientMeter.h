//=========================
/** Idea extract from  https://www.youtube.com/watch?v=ILMdPjFQ9ps  and  https://docs.juce.com/master/structAudioDeviceManager_1_1LevelMeter.html#ac8cb328944f5ac60cf570d246717aac8 */
//=====================
#pragma once

#include <JuceHeader.h>

namespace Gui
{
    class VerticalGradientMeter : public Component, public Timer
    {
    public:
        VerticalGradientMeter(std::function<float()>&& valueFunction);
        
        ~VerticalGradientMeter();
       
        void paint(Graphics& g) override;

        void resized() override;
        
        void paintOverChildren(::juce::Graphics& g) override;

        void timerCallback() override;
   
    private:
        std::function<float()> valueSupplier;
        ColourGradient gradient{};
        Image grill;
    };
}

