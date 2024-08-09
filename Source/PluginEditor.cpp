/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
KarplusStrong1AudioProcessorEditor::KarplusStrong1AudioProcessorEditor(KarplusStrong1AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    feedbackAttachment(*audioProcessor.apvts.getParameter("Feedback"), feedbackSliderRelay, audioProcessor.apvts.undoManager),
    dryGainAttachment(*audioProcessor.apvts.getParameter("Dry Gain"), dryGainSliderRelay, audioProcessor.apvts.undoManager),
    wetGainAttachment(*audioProcessor.apvts.getParameter("Wet Gain"), wetGainSliderRelay, audioProcessor.apvts.undoManager),
    colorAttachment(*audioProcessor.apvts.getParameter("Color"), colorSliderRelay, audioProcessor.apvts.undoManager),
    delaySamplesAttachment(*audioProcessor.apvts.getParameter("Delay Samples"), delaySamplesSliderRelay, audioProcessor.apvts.undoManager),
    noteNumberAttachment(*audioProcessor.apvts.getParameter("Note Number"), noteNumberSliderRelay, audioProcessor.apvts.undoManager)
{
    addAndMakeVisible(webComponent);
    webComponent.goToURL(localDevServerAddress);

    setResizable(true, true);

    setSize (400, 300);
}

KarplusStrong1AudioProcessorEditor::~KarplusStrong1AudioProcessorEditor()
{
}

//==============================================================================
void KarplusStrong1AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void KarplusStrong1AudioProcessorEditor::resized()
{
    webComponent.setBounds(getLocalBounds());
}
