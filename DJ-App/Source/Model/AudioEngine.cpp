#include "AudioEngine.h"

AudioEngine::AudioEngine(juce::AudioFormatManager& fm)
    : formatManager(fm), resampleSource(&transportSource, false, 2)
{
    // Setup EQ filters
    lowEQFilter.setCoefficients(juce::IIRCoefficients::makeLowShelf(44100, 250.0f, 1.0f, 1.0f));
    midEQFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(44100, 1000.0f, 1.0f, 1.0f));
    highEQFilter.setCoefficients(juce::IIRCoefficients::makeHighShelf(44100, 4000.0f, 1.0f, 1.0f));
}

AudioEngine::~AudioEngine()
{
    transportSource.setSource(nullptr);
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    
    // Update EQ filter coefficients for the actual sample rate
    lowEQFilter.setCoefficients(juce::IIRCoefficients::makeLowShelf(sampleRate, 250.0f, 1.0f, juce::Decibels::decibelsToGain(lowEQGain)));
    midEQFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(sampleRate, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(midEQGain)));
    highEQFilter.setCoefficients(juce::IIRCoefficients::makeHighShelf(sampleRate, 4000.0f, 1.0f, juce::Decibels::decibelsToGain(highEQGain)));
}

void AudioEngine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    // Check loop bounds before getting audio
    checkLoopBounds();
    
    // Get audio from the resample source
    resampleSource.getNextAudioBlock(bufferToFill);
    
    // Apply gain
    bufferToFill.buffer->applyGain(bufferToFill.startSample, bufferToFill.numSamples, currentGain);
    
    // Apply EQ
    applyEQ(*bufferToFill.buffer);
    
    // Update audio levels for meters
    updateAudioLevels(bufferToFill);
    
    // Trigger position callback
    if (onPositionChanged)
        onPositionChanged(getPosition());
}

void AudioEngine::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void AudioEngine::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    // Handle transport source state changes
}

void AudioEngine::loadTrack(const Track& track)
{
    stop();
    
    if (!track.isValid())
        return;
    
    auto* reader = formatManager.createReaderFor(track.getFile());
    if (reader != nullptr)
    {
        currentTrack = std::make_unique<Track>(track);
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        
        // Reset position and cue point
        setPosition(0.0);
        setCuePoint(0.0);
        
        if (onTrackLoaded)
            onTrackLoaded();
    }
}

void AudioEngine::play()
{
    transportSource.start();
    if (onPlaybackStarted)
        onPlaybackStarted();
}

void AudioEngine::pause()
{
    transportSource.stop();
}

void AudioEngine::stop()
{
    transportSource.stop();
    setPosition(0.0);
    if (onPlaybackStopped)
        onPlaybackStopped();
}

bool AudioEngine::isPlaying() const
{
    return transportSource.isPlaying();
}

bool AudioEngine::isPaused() const
{
    return !transportSource.isPlaying() && getPosition() > 0.0;
}

void AudioEngine::setPosition(double positionInSeconds)
{
    if (readerSource.get() != nullptr)
    {
        transportSource.setPosition(positionInSeconds);
    }
}

void AudioEngine::setPositionRelative(double position)
{
    setPosition(position * getTrackLength());
}

double AudioEngine::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double AudioEngine::getPositionRelative() const
{
    auto length = getTrackLength();
    return length > 0.0 ? getPosition() / length : 0.0;
}

double AudioEngine::getTrackLength() const
{
    return transportSource.getLengthInSeconds();
}

void AudioEngine::setGain(double gain)
{
    currentGain = juce::jlimit(0.0, 2.0, gain);
}

void AudioEngine::setSpeed(double speed)
{
    currentSpeed = juce::jlimit(0.5, 2.0, speed);
    resampleSource.setResamplingRatio(currentSpeed);
}

void AudioEngine::setPitch(double pitch)
{
    currentPitch = juce::jlimit(-12.0, 12.0, pitch);
    // Pitch shifting would require additional processing
    // For now, we'll just store the value
}

void AudioEngine::setLowEQ(double gain)
{
    lowEQGain = juce::jlimit(-20.0, 20.0, gain);
    lowEQFilter.setCoefficients(juce::IIRCoefficients::makeLowShelf(44100, 250.0f, 1.0f, juce::Decibels::decibelsToGain(lowEQGain)));
}

void AudioEngine::setMidEQ(double gain)
{
    midEQGain = juce::jlimit(-20.0, 20.0, gain);
    midEQFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(44100, 1000.0f, 1.0f, juce::Decibels::decibelsToGain(midEQGain)));
}

void AudioEngine::setHighEQ(double gain)
{
    highEQGain = juce::jlimit(-20.0, 20.0, gain);
    highEQFilter.setCoefficients(juce::IIRCoefficients::makeHighShelf(44100, 4000.0f, 1.0f, juce::Decibels::decibelsToGain(highEQGain)));
}

void AudioEngine::setCuePoint(double position)
{
    cuePoint = juce::jlimit(0.0, getTrackLength(), position);
}

void AudioEngine::jumpToCue()
{
    setPosition(cuePoint);
}

void AudioEngine::setLoopStart(double position)
{
    loopStart = juce::jlimit(0.0, getTrackLength(), position);
}

void AudioEngine::setLoopEnd(double position)
{
    loopEnd = juce::jlimit(loopStart, getTrackLength(), position);
}

void AudioEngine::enableLoop(bool enable)
{
    loopEnabled = enable;
}

void AudioEngine::updateAudioLevels(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto numChannels = bufferToFill.buffer->getNumChannels();
    auto numSamples = bufferToFill.numSamples;
    
    float currentRMS = 0.0f;
    float currentPeak = 0.0f;
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            auto sampleValue = std::abs(channelData[sample]);
            currentRMS += sampleValue * sampleValue;
            currentPeak = juce::jmax(currentPeak, sampleValue);
        }
    }
    
    rmsLevel = std::sqrt(currentRMS / (numSamples * numChannels));
    peakLevel = currentPeak;
}

void AudioEngine::applyEQ(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        lowEQFilter.processSamples(channelData, buffer.getNumSamples());
        midEQFilter.processSamples(channelData, buffer.getNumSamples());
        highEQFilter.processSamples(channelData, buffer.getNumSamples());
    }
}

void AudioEngine::checkLoopBounds()
{
    if (loopEnabled && loopEnd > loopStart)
    {
        auto currentPos = getPosition();
        if (currentPos >= loopEnd)
        {
            setPosition(loopStart);
        }
    }
}