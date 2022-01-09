/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace juce;

//==============================================================================
VST_PLUGAudioProcessorEditor::VST_PLUGAudioProcessorEditor (VST_PLUGAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(azimuthKnob = new Slider("Azimuth"));
    azimuthKnob->setSliderStyle(Slider::Rotary);
    azimuthKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(elevationKnob = new Slider("Elevation"));
    elevationKnob->setSliderStyle(Slider::Rotary);
    elevationKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(rollKnob = new Slider("Roll"));
    rollKnob->setSliderStyle(Slider::SliderStyle::Rotary);
    rollKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(widthKnob = new Slider("Width"));
    widthKnob->setSliderStyle(Slider::Rotary);
    widthKnob->setTextBoxStyle(Slider::NoTextBox, false, 100, 100);

    azimuthAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(),"azimuth", *azimuthKnob );
    elevationAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "elevation", *elevationKnob);
    rollAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "roll", *rollKnob);
    widthAttachment = new AudioProcessorValueTreeState::SliderAttachment(p.getState(), "width", *widthKnob);
    
    setSize (600, 500);
}

VST_PLUGAudioProcessorEditor::~VST_PLUGAudioProcessorEditor()
{
}

//==============================================================================
void VST_PLUGAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.drawText("Azimuth", ((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 5) - (100 / 2) + 50, 100, 100, Justification::centred, false);
    g.drawText(std::to_string((int)azimuthKnob->getValue()), ((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 5) - (100 / 2) + 5, 100, 100, Justification::centred, false);
    g.drawText("Elevation", ((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 5) - (100 / 2) + 50, 100, 100, Justification::centred, false);
    g.drawText(std::to_string((int)elevationKnob->getValue()), ((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 5) - (100 / 2) + 5, 100, 100, Justification::centred, false);
    g.drawText("Roll", ((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 5) - (100 / 2) + 50, 100, 100, Justification::centred, false);
    g.drawText(std::to_string((int)rollKnob->getValue()), ((getWidth() / 5) * 3) - (100 / 2), (getHeight() / 5) - (100 / 2) + 5, 100, 100, Justification::centred, false);
    g.drawText("Width", ((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 5) - (100 / 2) + 50, 100, 100, Justification::centred, false);
    g.drawText(std::to_string((int)widthKnob->getValue()), ((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 5) - (100 / 2) + 5, 100, 100, Justification::centred, false);
}

void VST_PLUGAudioProcessorEditor::resized()
{
    azimuthKnob->setBounds(((getWidth() / 5) * 1) - (100 / 2), (getHeight() / 5) - (100 / 2), 100, 100);
    elevationKnob->setBounds(((getWidth() / 5) * 2) - (100 / 2), (getHeight() / 5) - (100 / 2), 100, 100);
    rollKnob->setBounds(((getWidth() / 5) * 3) - (100 / 2), (getHeight() /5) - (100 / 2), 100, 100);
    widthKnob->setBounds(((getWidth() / 5) * 4) - (100 / 2), (getHeight() / 5) - (100 / 2), 100, 100);
}
