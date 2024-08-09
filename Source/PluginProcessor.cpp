/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KarplusStrong1AudioProcessor::KarplusStrong1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

KarplusStrong1AudioProcessor::~KarplusStrong1AudioProcessor()
{
}

//==============================================================================
const juce::String KarplusStrong1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KarplusStrong1AudioProcessor::acceptsMidi() const
{
    return true;
}

bool KarplusStrong1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool KarplusStrong1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double KarplusStrong1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KarplusStrong1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int KarplusStrong1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void KarplusStrong1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String KarplusStrong1AudioProcessor::getProgramName (int index)
{
    return {};
}

void KarplusStrong1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void KarplusStrong1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto delayBufferSize = sampleRate * 2;   // 2s data
    delayBuffer.setSize(2, delayBufferSize, false, true);

    writePosition = 0;
}

void KarplusStrong1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool KarplusStrong1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void KarplusStrong1AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals; // Avoids denormals which can lead to performance issues

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();
    int delayBufferSize = delayBuffer.getNumSamples();

    // Clear any unused output channels to avoid noisy output
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Shared parameter values fetched once per block
    double frequency = juce::MidiMessage::getMidiNoteInHertz(*apvts.getRawParameterValue("Note Number"));
    setDelaySamples(calculateDelaySamples(frequency));

    int delaySamples = *apvts.getRawParameterValue("Delay Samples");
    float color = *apvts.getRawParameterValue("Color");
    float currentFeedback = *apvts.getRawParameterValue("Feedback");
    currentFeedback = decayMap(currentFeedback);
    float dryGain = *apvts.getRawParameterValue("Dry Gain");
    float wetGain = *apvts.getRawParameterValue("Wet Gain");
    bool squareMode = *apvts.getRawParameterValue("Square Mode");

    if (squareMode) {
        currentFeedback *= -1;
    }

    // Process each sample in the block
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {

        // Calculate read position based on the shared write position
        int readPosition = (writePosition - delaySamples + delayBufferSize) % delayBufferSize;

        // Each channel processes the same sample index within the block
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            auto* delayData = delayBuffer.getWritePointer(channel);
            float dry = channelData[sample];
            
            float total = dry + currentFeedback * (delayData[readPosition] * color + delayData[(readPosition - 1 + delayBufferSize) % delayBufferSize] * (1 - color));
            delayData[writePosition] = total;
            float wet = total - dry;

            channelData[sample] = wet * juce::Decibels::decibelsToGain(wetGain, -60.f) + dry * juce::Decibels::decibelsToGain(dryGain, -60.f);
        }

        // Increment and wrap the shared write position once per sample, not per channel
        writePosition = (writePosition + 1) % delayBufferSize;
    }
}



//==============================================================================
bool KarplusStrong1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* KarplusStrong1AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void KarplusStrong1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void KarplusStrong1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout KarplusStrong1AudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    using namespace juce;

    layout.add(std::make_unique<AudioParameterFloat>("Feedback",
        "Feedback",
        NormalisableRange<float>(0, 1, 0.0001, 1), 0.1));

    layout.add(std::make_unique<AudioParameterFloat>("Dry Gain",
        "Dry Gain",
        NormalisableRange<float>(-60, 12, 0.1, 1), 0));

    layout.add(std::make_unique<AudioParameterFloat>("Wet Gain",
        "Wet Gain",
        NormalisableRange<float>(-60, 12, 0.1, 1), -15));

    layout.add(std::make_unique<AudioParameterFloat>("Color",
        "Color",
        NormalisableRange<float>(0.5, 1, 0.0001, 1), 0.5));

    layout.add(std::make_unique<AudioParameterInt>("Delay Samples",
        "Delay Samples",
        10, 10000, 200));

    layout.add(std::make_unique<AudioParameterInt>("Note Number",
        "Note Number",
        0, 128, 60));

    layout.add(std::make_unique<AudioParameterBool>("Square Mode", "Square Mode", false));

    




    return layout;
}

void KarplusStrong1AudioProcessor::fillBufferWithNoise()
{
    delayBuffer.clear();
    int delaySamples = *apvts.getRawParameterValue("Delay Samples");
    int delayBufferSize = delayBuffer.getNumSamples();
    auto numChannels = delayBuffer.getNumChannels();
    
    //Should create a nice stereo effect since randomization is independent for channels
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* writePointer = delayBuffer.getWritePointer(channel);
        for (int i = delayBufferSize - delaySamples; i < delayBufferSize; ++i)
        {
            writePointer[i] = random.nextFloat() * 2.0f - 1.0f; // Fill with noise between -1 and 1
        }
    } 
}

int KarplusStrong1AudioProcessor::calculateDelaySamples(double freq)
{
    double sampleRate = getSampleRate();
    return static_cast<int>(sampleRate/freq - 0.5); 
}

void KarplusStrong1AudioProcessor::setDelaySamples(int delay)
{
    // Ensure this method is called on the audio thread or use a MessageManagerLock if called from another thread
    auto* delaySamplesPointer = apvts.getParameter("Delay Samples");
    if (delaySamplesPointer != nullptr)
    {
        // Get the parameter range
        const auto& range = delaySamplesPointer->getNormalisableRange();

        // Convert the integer delay to a normalized value
        float normalizedDelay = range.convertTo0to1(static_cast<float>(delay));

        // Set the value, notifying the host
        delaySamplesPointer->setValueNotifyingHost(normalizedDelay);
    }
}

float KarplusStrong1AudioProcessor::decayMap(float decay)
{
    const float denominator = 0.9917;
    return (1 - std::exp(-4.8 * decay)) / denominator;
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KarplusStrong1AudioProcessor();
}
