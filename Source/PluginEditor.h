#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class SingleStringTransposeAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                   private juce::ComboBox::Listener,
                                                   private juce::Slider::Listener
{
public:
    SingleStringTransposeAudioProcessorEditor (SingleStringTransposeAudioProcessor&);
    ~SingleStringTransposeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SingleStringTransposeAudioProcessor& audioProcessor;

    juce::ComboBox stringSelector;   // For selecting guitar string
    juce::Slider transposeSlider;    // For selecting semitone shift

    void comboBoxChanged(juce::ComboBox* box) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SingleStringTransposeAudioProcessorEditor)
};
