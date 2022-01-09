/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VST_PLUGAudioProcessorEditor::VST_PLUGAudioProcessorEditor (VST_PLUGAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    mAzimuthSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    mAzimuthSlider.setRange(0,360,1);
    mAzimuthSlider.setValue(0);
    addAndMakeVisible(mAzimuthSlider);
    setSize (600, 600);
}

VST_PLUGAudioProcessorEditor::~VST_PLUGAudioProcessorEditor()
{
}

//==============================================================================
void VST_PLUGAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("SPD project Nice", getLocalBounds(), juce::Justification::centred, 1);
}

void VST_PLUGAudioProcessorEditor::resized()
{
    mAzimuthSlider.setBounds(getWidth()/2 -100, getHeight() / 2- 100, 100, 150);
}
