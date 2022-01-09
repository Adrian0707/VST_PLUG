/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class VST_PLUGAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    VST_PLUGAudioProcessorEditor (VST_PLUGAudioProcessor&);
    ~VST_PLUGAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider mAzimuthSlider;
    VST_PLUGAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST_PLUGAudioProcessorEditor)
};
