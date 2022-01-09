/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VST_PLUGAudioProcessor::VST_PLUGAudioProcessor()
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

VST_PLUGAudioProcessor::~VST_PLUGAudioProcessor()
{
}

//==============================================================================
const juce::String VST_PLUGAudioProcessor::getName() const
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

const juce::String VST_PLUGAudioProcessor::getProgramName (int index)
{
    return {};
}

void VST_PLUGAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VST_PLUGAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
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

void VST_PLUGAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
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

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool VST_PLUGAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VST_PLUGAudioProcessor::createEditor()
{
    //return new VST_PLUGAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void VST_PLUGAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VST_PLUGAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout VST_PLUGAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Azimuth",
        "Azimuth",
        juce::NormalisableRange<float>(0, 360.f, 1.f, 1.f), 0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Elevation",
        "Elevation",
        juce::NormalisableRange<float>(0, 360.f, 1.f, 1.f), 0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Roll",
        "Roll",
        juce::NormalisableRange<float>(0, 360.f, 1.f, 1.f), 0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Width",
        "Width",
        juce::NormalisableRange<float>(0, 360.f, 1.f, 1.f), 0.f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "W",
        "W",
        juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f),0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "X",
        "X",
        juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Y",
        "Y",
        juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Z",
        "Z",
        juce::NormalisableRange<float>(-1.f, 1.f, 0.01f, 1.f), 0.f));

    juce::StringArray stringArray;
    for (int i = 0; i < 4; i++) {
        juce::String str;
        str << (12 + i * 12);
        str << "db\OCT";
        stringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCust Slope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VST_PLUGAudioProcessor();
}
