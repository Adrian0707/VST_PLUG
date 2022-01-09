/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//using namespace juce;

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

    juce::ScopedPointer<juce::Slider> azimuthKnob;
    juce::ScopedPointer<juce::Slider> elevationKnob;
    juce::ScopedPointer<juce::Slider> rollKnob;
    juce::ScopedPointer<juce::Slider> widthKnob;

    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> azimuthAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> elevationAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> rollAttachment;
    juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
   //juce::Slider mAzimuthSlider;
    //juce::TextLayout textLayout;
    VST_PLUGAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST_PLUGAudioProcessorEditor)
};
