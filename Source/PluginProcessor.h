/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "HRTFContainer.h"
#include "HRIRFilter.h"
#include "Crossover.h"

using namespace juce;

//==============================================================================
/**
*/
class VST_PLUGAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    VST_PLUGAudioProcessor();
    ~VST_PLUGAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState& getState();
private:
    ScopedPointer<AudioProcessorValueTreeState> state;

    HRIRFilter hrirFilterL;
    HRIRFilter hrirFilterR;
    HRTFContainer hrtfContainer;
    AudioSampleBuffer crossoverOutput;
    Crossover crossover;
    AudioSampleBuffer monoInputBuffer;
    bool hrirLoaded;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST_PLUGAudioProcessor)
};
