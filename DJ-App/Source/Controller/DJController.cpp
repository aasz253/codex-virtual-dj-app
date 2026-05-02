#include "DJController.h"

DJController::DJController() : thumbnailCache(100)
{
    formatManager.registerBasicFormats();
    setupAudioEngines();
    
    // Setup audio channels
    setAudioChannels(0, 2);
}

DJController::~DJController()
{
    shutdownAudio();
}

void DJController::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    deck1->prepareToPlay(samplesPerBlockExpected, sampleRate);
    deck2->prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJController::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Clear the buffer first
    bufferToFill.clearActiveBufferRegion();
    
    // Create temporary buffers for each deck
    juce::AudioBuffer<float> deck1Buffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioBuffer<float> deck2Buffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    
    // Get audio from each deck
    juce::AudioSourceChannelInfo deck1Info(&deck1Buffer, 0, bufferToFill.numSamples);
    juce::AudioSourceChannelInfo deck2Info(&deck2Buffer, 0, bufferToFill.numSamples);
    
    deck1->getNextAudioBlock(deck1Info);
    deck2->getNextAudioBlock(deck2Info);
    
    // Apply crossfader mixing
    double deck1Gain = calculateDeckGain(0);
    double deck2Gain = calculateDeckGain(1);
    
    // Mix the decks into the output buffer
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        auto* outputData = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        auto* deck1Data = deck1Buffer.getReadPointer(channel);
        auto* deck2Data = deck2Buffer.getReadPointer(channel);
        
        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            outputData[sample] = (deck1Data[sample] * deck1Gain + deck2Data[sample] * deck2Gain) * masterGain;
        }
    }
    
    // Handle recording if active
    if (recordingWriter != nullptr)
    {
        recordingWriter->writeFromAudioSampleBuffer(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
    }
    
    // Update UI callbacks
    handleDeckLevelsChange(0);
    handleDeckLevelsChange(1);
}

void DJController::releaseResources()
{
    deck1->releaseResources();
    deck2->releaseResources();
    mixerSource.releaseResources();
    
    if (recordingWriter != nullptr)
    {
        recordingWriter.reset();
    }
}

void DJController::setupAudioEngines()
{
    deck1 = std::make_unique<AudioEngine>(formatManager);
    deck2 = std::make_unique<AudioEngine>(formatManager);
    
    // Setup callbacks for deck 1
    deck1->onPositionChanged = [this](double position) {
        handleDeckPositionChange(0, position);
    };
    
    // Setup callbacks for deck 2
    deck2->onPositionChanged = [this](double position) {
        handleDeckPositionChange(1, position);
    };
    
    // Setup playlist callbacks
    playlistManager.onPlaylistChanged = [this]() {
        if (onPlaylistChanged)
            onPlaylistChanged();
    };
}

void DJController::loadTrackToDeck(int deckIndex, const Track& track)
{
    if (deckIndex == 0)
        deck1->loadTrack(track);
    else if (deckIndex == 1)
        deck2->loadTrack(track);
}

void DJController::playDeck(int deckIndex)
{
    if (deckIndex == 0)
        deck1->play();
    else if (deckIndex == 1)
        deck2->play();
}

void DJController::pauseDeck(int deckIndex)
{
    if (deckIndex == 0)
        deck1->pause();
    else if (deckIndex == 1)
        deck2->pause();
}

void DJController::stopDeck(int deckIndex)
{
    if (deckIndex == 0)
        deck1->stop();
    else if (deckIndex == 1)
        deck2->stop();
}

void DJController::setDeckPosition(int deckIndex, double position)
{
    if (deckIndex == 0)
        deck1->setPositionRelative(position);
    else if (deckIndex == 1)
        deck2->setPositionRelative(position);
}

void DJController::setDeckGain(int deckIndex, double gain)
{
    if (deckIndex == 0)
        deck1->setGain(gain);
    else if (deckIndex == 1)
        deck2->setGain(gain);
}

void DJController::setDeckSpeed(int deckIndex, double speed)
{
    if (deckIndex == 0)
        deck1->setSpeed(speed);
    else if (deckIndex == 1)
        deck2->setSpeed(speed);
}

void DJController::setDeckEQ(int deckIndex, double low, double mid, double high)
{
    if (deckIndex == 0)
    {
        deck1->setLowEQ(low);
        deck1->setMidEQ(mid);
        deck1->setHighEQ(high);
    }
    else if (deckIndex == 1)
    {
        deck2->setLowEQ(low);
        deck2->setMidEQ(mid);
        deck2->setHighEQ(high);
    }
}

void DJController::setCrossfader(double position)
{
    crossfaderPosition = juce::jlimit(-1.0, 1.0, position);
    
    if (onCrossfaderChanged)
        onCrossfaderChanged(crossfaderPosition);
}

void DJController::setMasterGain(double gain)
{
    masterGain = juce::jlimit(0.0, 2.0, gain);
}

void DJController::setCueGain(double gain)
{
    cueGain = juce::jlimit(0.0, 2.0, gain);
}

void DJController::loadPlaylistFromDirectory(const juce::File& directory)
{
    playlistManager.loadTracksFromDirectory(directory);
}

void DJController::loadPlaylistFromFiles(const juce::Array<juce::File>& files)
{
    playlistManager.loadTracksFromFiles(files);
}

void DJController::syncDecks()
{
    // Get BPM from both decks' current tracks
    auto* track1 = deck1->getCurrentTrack();
    auto* track2 = deck2->getCurrentTrack();
    
    if (track1 && track2)
    {
        int bpm1 = track1->getBPM();
        int bpm2 = track2->getBPM();
        
        if (bpm1 > 0 && bpm2 > 0)
        {
            // Sync deck 2 to deck 1's BPM
            double speedRatio = static_cast<double>(bpm1) / static_cast<double>(bpm2);
            deck2->setSpeed(speedRatio);
        }
    }
}

void DJController::enableBeatSync(bool enable)
{
    beatSyncEnabled = enable;
}

void DJController::startRecording(const juce::File& outputFile)
{
    stopRecording(); // Stop any existing recording
    
    recordingFile = outputFile;
    
    // Create WAV format writer
    juce::WavAudioFormat wavFormat;
    auto* writer = wavFormat.createWriterFor(new juce::FileOutputStream(outputFile),
                                           44100.0, // Sample rate
                                           2,       // Channels
                                           16,      // Bits per sample
                                           {},      // Metadata
                                           0);      // Quality option
    
    recordingWriter.reset(writer);
}

void DJController::stopRecording()
{
    recordingWriter.reset();
}

bool DJController::isRecording() const
{
    return recordingWriter != nullptr;
}

void DJController::enableAutoCrossfade(bool enable)
{
    autoCrossfadeEnabled = enable;
}

void DJController::setAutoCrossfadeTime(double seconds)
{
    autoCrossfadeTime = juce::jmax(1.0, seconds);
}

double DJController::calculateDeckGain(int deckIndex) const
{
    // Calculate gain based on crossfader position
    if (deckIndex == 0) // Deck 1
    {
        // When crossfader is at -1.0 (full left), deck 1 is at full volume
        // When crossfader is at 1.0 (full right), deck 1 is silent
        return juce::jmax(0.0, (1.0 - crossfaderPosition) * 0.5);
    }
    else // Deck 2
    {
        // When crossfader is at -1.0 (full left), deck 2 is silent
        // When crossfader is at 1.0 (full right), deck 2 is at full volume
        return juce::jmax(0.0, (1.0 + crossfaderPosition) * 0.5);
    }
}

void DJController::handleDeckPositionChange(int deckIndex, double position)
{
    if (onDeckPositionChanged)
        onDeckPositionChanged(deckIndex, position);
}

void DJController::handleDeckLevelsChange(int deckIndex)
{
    if (onDeckLevelsChanged)
    {
        AudioEngine* deck = (deckIndex == 0) ? deck1.get() : deck2.get();
        if (deck)
        {
            float rms = deck->getRMSLevel();
            float peak = deck->getPeakLevel();
            onDeckLevelsChanged(deckIndex, rms, peak);
        }
    }
}

// Additional deck control implementations
void DJController::togglePlay(int deckIndex)
{
    AudioEngine* deck = (deckIndex == 0) ? deck1.get() : deck2.get();
    if (deck)
    {
        if (deck->isPlaying())
            pauseDeck(deckIndex);
        else
            playDeck(deckIndex);
    }
}

void DJController::cue(int deckIndex)
{
    AudioEngine* deck = (deckIndex == 0) ? deck1.get() : deck2.get();
    if (deck)
    {
        deck->setCuePoint(deck->getPosition());
        setDeckPosition(deckIndex, deck->getCuePoint());
    }
}

void DJController::loadTrack(int deckIndex)
{
    auto chooser = std::make_unique<juce::FileChooser>("Select an audio file to load...",
                                                      juce::File::getSpecialLocation(juce::File::userMusicDirectory),
                                                      "*.wav;*.mp3;*.aiff;*.flac;*.ogg;*.m4a");
    
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles, [this, deckIndex](const juce::FileChooser& fc)
    {
        juce::File selectedFile = fc.getResult();
        
        if (selectedFile.existsAsFile())
        {
            Track track(selectedFile);
            
            if (track.isValid())
            {
                loadTrackToDeck(deckIndex, track);
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon,
                    "Error",
                    "Could not load the selected audio file. Please make sure it's a valid audio format.");
            }
        }
    });
}

void DJController::toggleSync(int deckIndex)
{
    // Toggle BPM sync for the deck
    enableBeatSync(!isBeatSyncEnabled());
}

void DJController::toggleLoop(int deckIndex)
{
    AudioEngine* deck = (deckIndex == 0) ? deck1.get() : deck2.get();
    if (deck)
    {
        deck->enableLoop(!deck->isLoopEnabled());
    }
}

void DJController::adjustPosition(int deckIndex, double delta)
{
    AudioEngine* deck = (deckIndex == 0) ? deck1.get() : deck2.get();
    if (deck)
    {
        double currentPos = deck->getPosition();
        double newPos = juce::jlimit(0.0, 1.0, currentPos + delta);
        setDeckPosition(deckIndex, newPos);
    }
}

void DJController::adjustSpeed(int deckIndex, double delta)
{
    AudioEngine* deck = (deckIndex == 0) ? deck1.get() : deck2.get();
    if (deck)
    {
        double currentSpeed = deck->getSpeed();
        double newSpeed = juce::jlimit(0.1, 3.0, currentSpeed + delta);
        setDeckSpeed(deckIndex, newSpeed);
    }
}

void DJController::stop(int deckIndex)
{
    stopDeck(deckIndex);
}