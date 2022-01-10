/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
using namespace juce;

//==============================================================================
/**
*/
class VST_PLUGAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    VST_PLUGAudioProcessorEditor (VST_PLUGAudioProcessor&);
    ~VST_PLUGAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:

    ScopedPointer<Slider> azimuthKnob;
    ScopedPointer<Slider> elevationKnob;
    ScopedPointer<Slider> rollKnob;
    ScopedPointer<Slider> widthKnob;

    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> azimuthAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> elevationAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> rollAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> widthAttachment;

    VST_PLUGAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST_PLUGAudioProcessorEditor)
};
