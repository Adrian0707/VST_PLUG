/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace juce;

//==============================================================================
VST_PLUGAudioProcessor::VST_PLUGAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    state = new AudioProcessorValueTreeState(*this, nullptr);
    state->createAndAddParameter("azimuth", "Azimuth", "Azimuth", NormalisableRange<float>(0.f, 360.f, 1.f), 0, nullptr, nullptr);
    state->createAndAddParameter("elevation", "Elevation", "Elevation", NormalisableRange<float>(-90.f, 90.f, 1.f), 0, nullptr, nullptr);
    state->createAndAddParameter("roll", "Roll", "Roll", NormalisableRange<float>(0.f, 360.f, 1.f), 0, nullptr, nullptr);
    state->createAndAddParameter("width", "Width", "Width", NormalisableRange<float>(0.f, 120.f, 1.f), 0, nullptr, nullptr);

    state->state = ValueTree("azimuth");
    state->state = ValueTree("elevation");
    state->state = ValueTree("roll");
    state->state = ValueTree("width");
}

VST_PLUGAudioProcessor::~VST_PLUGAudioProcessor()
{
}

//==============================================================================
const String VST_PLUGAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VST_PLUGAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VST_PLUGAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VST_PLUGAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VST_PLUGAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VST_PLUGAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VST_PLUGAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VST_PLUGAudioProcessor::setCurrentProgram (int index)
{
}

const String VST_PLUGAudioProcessor::getProgramName (int index)
{
    return {};
}

void VST_PLUGAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VST_PLUGAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VST_PLUGAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VST_PLUGAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void VST_PLUGAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    return;
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float azimuth = *state->getRawParameterValue("azimuth");
    float elevation = *state->getRawParameterValue("elevation");
    float roll = *state->getRawParameterValue("roll");
    float width = *state->getRawParameterValue("width");
    azimuth = (azimuth * 3.14) / 180;
    elevation = (elevation * 3.14) / 180;
    roll = (roll * 3.14) / 180;
    width = (width * 3.14) / 180;

    // FROM OTHER PROJECT

    auto bufferLChannel = buffer.getWritePointer(0);
    auto bufferRChannel = buffer.getWritePointer(1);
    const auto bufferLength = buffer.getNumSamples();

    // downmix to mono in case of a stereo input
    // by adding from the right channel to left channel
    if (getTotalNumInputChannels() == 2)
    {
        buffer.addFrom(0, 0, bufferRChannel, bufferLength);
        buffer.applyGain(0.5f);
    }
    monoInputBuffer.copyFrom(0, 0, buffer, 0, 0, bufferLength);

    // split the input signal into two bands, only freqs above crossover's f0
    // will be spatialized
    crossover.process(buffer, 0, crossoverOutput);

    // we need to copy the hi-pass input to buffers
    buffer.copyFrom(0, 0, crossoverOutput, Crossover::hiPassChannelIndex, 0, bufferLength);
    buffer.copyFrom(1, 0, crossoverOutput, Crossover::hiPassChannelIndex, 0, bufferLength);

    // actual hrir filtering
    const auto& hrir = hrtfContainer.hrir();
    hrirFilterL.setImpulseResponse(hrir.leftEarIR);
    hrirFilterR.setImpulseResponse(hrir.rightEarIR);
    hrirFilterL.process(bufferLChannel, bufferLength);
    hrirFilterR.process(bufferRChannel, bufferLength);

    //// fill stereo output
    //const auto wetRatio = wetPercentParam->getValueAndMarkRead() / 100;
    //const auto dryRatio = 1 - wetRatio;
    //const auto lowPassInput = crossoverOutput.getReadPointer(Crossover::loPassChannelIndex);
    //for (auto i = 0; i < bufferLength; i++)
    //{
    //    const auto monoIn = *monoInputBuffer.getReadPointer(0, i);
    //    const auto gain = Decibels::decibelsToGain(gainDbParam->getValueAndMarkRead());
    //    bufferLChannel[i] = gain * wetRatio * (lowPassInput[i] + bufferLChannel[i]) + dryRatio * monoIn;
    //    bufferRChannel[i] = gain * wetRatio * (lowPassInput[i] + bufferRChannel[i]) + dryRatio * monoIn;
    //}
    //  
    ////
  
    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer(channel);

    //    for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
    //        //channelData[sample] = channelData[sample]; //* mAzimuth;


    //        //float cleanSig = *channelData;
    //        //*channelData *= azimuth * elevation;
    //        //*channelData = (((((2.f / 3.14f) * atan(*channelData)) * roll) + (cleanSig * (1.f / roll))) / 2) * width;
    //        
    //        if (channel == 0) {
    //            *channelData = (sqrt(2) * W * *channelData + X * *channelData - Y * *channelData) * sqrt(8);
    //        }
    //        if (channel == 1) {
    //            *channelData = (sqrt(2) * W * *channelData - X * *channelData + Y * *channelData) * sqrt(8);
    //        }
    //        channelData++;
    //    }
    //}
}

AudioProcessorValueTreeState& VST_PLUGAudioProcessor::getState() {
    return *state;
}

//==============================================================================
bool VST_PLUGAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VST_PLUGAudioProcessor::createEditor()
{
    return new VST_PLUGAudioProcessorEditor (*this);
    //return new GenericAudioProcessorEditor(*this);
}

//==============================================================================
void VST_PLUGAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    MemoryOutputStream stream(destData, false);
    state->state.writeToStream(stream);
}

void VST_PLUGAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ValueTree tree = ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid()) {
        state->state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VST_PLUGAudioProcessor();
}
