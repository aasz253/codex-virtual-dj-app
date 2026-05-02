#include "MainComponent.h"

MainComponent::MainComponent(AudioFormatManager& formatManagerToUse, AudioThumbnailCache& cacheToUse) : audioThumb(512, formatManager, thumbCache)
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (0, 2);
    }
    //Make components visible
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent1);
    addAndMakeVisible(playlistComponent2);
    
    /**Inform the code about basic formats such as MP3**/
    formatManager.registerBasicFormats();
    
    ///======================================
    // Directory creation and file search logic here
    File dirToCreate = File::getCurrentWorkingDirectory().getChildFile("directory/to/create");
    if (!dirToCreate.exists()) {
        auto creationResult = dirToCreate.createDirectory();
        if (!creationResult.wasOk()) {
            std::cerr << "Failed to create directory: " << creationResult.getErrorMessage() << std::endl;
            return; // Handle the error as needed
        }
    }

    File dirToSearch = dirToCreate; // Use the directory you've just ensured exists
    playlistComponent1.songs = dirToSearch.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.mp3");
    playlistComponent2.songs = dirToSearch.findChildFiles(File::TypesOfFileToFind::findFiles, false, "*.mp3");
    //=======================================
    
    // Assuming you have a method in PlaylistComponent to add these files
    playlistComponent1.fileLoadedCallback(playlistComponent1.songs);
    playlistComponent2.fileLoadedCallback(playlistComponent1.songs);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    /**Supply audio sys**/
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
    
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
    g.setFont(30.0f);
    g.setColour(juce::Colours::white);
    g.drawText("Welcome to DJChelApp	", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{
    //setting boundaries of components
    deckGUI1.setBounds(0, 0, getWidth()/2, getHeight()/2);
    deckGUI2.setBounds(getWidth()/2, 0, getWidth()/2, getHeight()/2);
    playlistComponent1.setBounds(0, getHeight()/2, getWidth()/2, getHeight()/2);
    playlistComponent2.setBounds(getWidth()/2, getHeight()/2, getWidth()/2, getHeight()/2);
}


