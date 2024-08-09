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
class KarplusStrong1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    KarplusStrong1AudioProcessorEditor (KarplusStrong1AudioProcessor&);
    ~KarplusStrong1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    KarplusStrong1AudioProcessor& audioProcessor;

    juce::WebSliderRelay feedbackSliderRelay{webComponent, "Feedback" };
    juce::WebSliderRelay dryGainSliderRelay{ webComponent, "Dry Gain" };
    juce::WebSliderRelay wetGainSliderRelay{ webComponent, "Wet Gain" };
    juce::WebSliderRelay colorSliderRelay{ webComponent, "Color" };
    juce::WebSliderRelay delaySamplesSliderRelay{ webComponent, "Delay Samples" };
    juce::WebSliderRelay noteNumberSliderRelay{ webComponent, "Note Number" };

    juce::WebBrowserComponent webComponent{ juce::WebBrowserComponent::Options{}
                                         .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
                                         .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
                                                                      .withUserDataFolder(juce::File::getSpecialLocation(juce::File::SpecialLocationType::tempDirectory)))
                                         .withNativeIntegrationEnabled()
                                         .withOptionsFrom(feedbackSliderRelay)
                                         .withOptionsFrom(dryGainSliderRelay)
                                         .withOptionsFrom(wetGainSliderRelay)
                                         .withOptionsFrom(colorSliderRelay)
                                         .withOptionsFrom(delaySamplesSliderRelay)
                                         .withOptionsFrom(noteNumberSliderRelay)};

    juce::WebSliderParameterAttachment feedbackAttachment;
    juce::WebSliderParameterAttachment dryGainAttachment;
    juce::WebSliderParameterAttachment wetGainAttachment;
    juce::WebSliderParameterAttachment noteNumberAttachment;
    juce::WebSliderParameterAttachment colorAttachment;
    juce::WebSliderParameterAttachment delaySamplesAttachment;

    const juce::String localDevServerAddress = "http://localhost:3000/";

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KarplusStrong1AudioProcessorEditor)
};
