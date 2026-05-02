/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "View/MainView.h"

//==============================================================================
class NewProjectApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    NewProjectApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public juce::DocumentWindow,
                           public juce::AudioAppComponent
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            
            // Create and set the main view
            mainView = std::make_unique<MainView>();
            setContentOwned (mainView.release(), true);
            
            // Setup audio
            setAudioChannels (0, 2); // No input, stereo output

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            DocumentWindow::setResizable (true, true);
            DocumentWindow::centreWithSize (1400, 900);
           #endif

            DocumentWindow::setVisible (true);
        }

        void closeButtonPressed() override
        {
            // Shutdown audio before closing
            shutdownAudio();
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
        // AudioAppComponent methods
        void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
        {
            if (auto* mainView = dynamic_cast<MainView*>(getContentComponent()))
                mainView->prepareToPlay(samplesPerBlockExpected, sampleRate);
        }
        
        void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
        {
            if (auto* mainView = dynamic_cast<MainView*>(getContentComponent()))
                mainView->getNextAudioBlock(bufferToFill);
            else
                bufferToFill.clearActiveBufferRegion();
        }
        
        void releaseResources() override
        {
            if (auto* mainView = dynamic_cast<MainView*>(getContentComponent()))
                mainView->releaseResources();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        std::unique_ptr<MainView> mainView;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (NewProjectApplication)
