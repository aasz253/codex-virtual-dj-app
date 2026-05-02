#include "DJAudioPlayer.h"

//constructor
DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager,AudioThumbnailCache& cacheToUse)
: formatManager(_formatManager),wave(_formatManager, cacheToUse)
{
}

//Destructor
DJAudioPlayer::~DJAudioPlayer()
{
}

//AudioSource interface
//birth stage
void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // Prepare the transport and resample sources for playback
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
};

//equivalent to draw() in p5. It runs until app is quit
void DJAudioPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    calculateCurrentLevel(bufferToFill);
    //delegate task
    resampleSource.getNextAudioBlock(bufferToFill);
    
    // Simple RMS level calculation 
    auto* channelData = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
    float sum = 0.0f;
    for (auto i = 0; i < bufferToFill.numSamples; ++i) {
        sum += channelData[i] * channelData[i];
    }
    currentRMSLevel = std::sqrt(sum / bufferToFill.numSamples);
};

//called when app exits, used to free up audio resources
void DJAudioPlayer::releaseResources()
{
    // Release audio resources held by transport and resample sources
    transportSource.releaseResources();
    resampleSource.releaseResources();
};

void DJAudioPlayer::loadFile(File audioFile)
{
    bool fileAlreadyLoaded = loadedFiles.contains(audioFile);
    if (!fileAlreadyLoaded)
    {
        loadedFiles.add(audioFile); // Add to the array only if it's not already loaded
    }

    auto* reader = formatManager.createReaderFor(audioFile);
    if (reader != nullptr) // If a good file
    {
        /**AudioFormatReaderSource**/
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
        /**Set to transportSource**/
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        /**Reset smart pointer if something goes wrong**/
        readerSource.reset(newSource.release());

        currentLoadedFile = audioFile; // Update the current file regardless

        /**Notify PlaylistComponent about the loaded file only if it's a new file**/
        if (!fileAlreadyLoaded && fileLoadedCallback)
        {
            fileLoadedCallback(audioFile);// Invoke the callback function
        }
    }
    else
    {
        // Log an error if the file could not be found or read
        std::cout << "DJAudioPlayer::loadFile - File not found" << std::endl;
    }

};
void DJAudioPlayer::setGain(double gain)
{
        /**This way we can ensure the vol limit is respected and provide a better user experience**/
        if(gain < 0 || gain > 1.0)
        {
            // Log an error if the gain is outside the valid range
            std::cout<< "DJAudioPlayer::setGain - gain should be between 0 and 1." << std::endl;
        }else{
            transportSource.setGain(gain);
        }
};
void DJAudioPlayer::setSpeed(double ratio)
{
    // Check for valid speed ratio range and set the speed if valid
    if(ratio < 0 || ratio > 100.0)
    {
        // Log an error if the speed ratio is outside the valid range
        std::cout<< "DJAudioPlayer::setSpeed - ratio speed should be between 0 and 100.0" << std::endl;
    }else{
        resampleSource.setResamplingRatio(ratio);
    }
};

//set position of song
void DJAudioPlayer::setPosition(double posInSecs)
{
    // Set the playback position in seconds
    transportSource.setPosition(posInSecs);
};

//Deals with length of song
//Reduces coupling 
void DJAudioPlayer::setPositionRelative(double pos)
{
    if(pos < 0 || pos > 1.0)// Check for valid relative position range and set the position if valid
    {
        // Log an error if the position is outside the valid range
        std::cout<< "DJAudioPlayer::setPositionRelative - pos should be between 0 and 1" << std::endl;
    }else{
        double posInSecs = transportSource.getLengthInSeconds()*pos;
        setPosition(posInSecs); // Convert relative position to seconds and set it
    }
};

void DJAudioPlayer::start(){transportSource.start();}; // Start playback
void DJAudioPlayer::stop(){transportSource.stop();}; // Stop playback

double DJAudioPlayer::getPositionRelative()
{
    // Calculate and return the current playback position as a relative value
    return transportSource.getCurrentPosition()/transportSource.getLengthInSeconds();
}

void DJAudioPlayer::setFileLoadedCallback(FileLoadedCallBack callback)
{
    // Set the callback function to be called when a file is loaded
    fileLoadedCallback = callback;
}

void DJAudioPlayer::loadFileIntoWaveform(const juce::File& file)
{
    //logic to load file into the waveform display
    if(!file.existsAsFile()) return;
    
    //load new file
    wave.loadFile(file);
}


void DJAudioPlayer::calculateCurrentLevel(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    // Calculate the current audio level for visualization purposes
    float newLevel = 0.0f;
    auto* buffer = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
    for (auto i = 0; i < bufferToFill.numSamples; ++i) {
        auto level = std::abs(buffer[i]);
        if (level > currentRMSLevel) currentRMSLevel = level;// Find the max level in the current block
    }
    currentRMSLevel = currentRMSLevel * 0.9f + newLevel * 0.1f; // Simple smoothing
    // You might want to smooth or decay the level value over time.
}

bool DJAudioPlayer::isPlaying() const {
    return transportSource.isPlaying();// Return whether the transport source is currently playing
}
double DJAudioPlayer::getCurrentPosition() const {
    return transportSource.getCurrentPosition();// Return the current playback position in seconds
}
double DJAudioPlayer::getLengthInSeconds() const {
    if (readerSource != nullptr) {
        // Calculate and return the total length of the currently loaded file in seconds
        return readerSource->getTotalLength() / readerSource->getAudioFormatReader()->sampleRate;
    }
    return 0.0;
}
