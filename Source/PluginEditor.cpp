#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SingleStringTransposeAudioProcessorEditor::SingleStringTransposeAudioProcessorEditor (SingleStringTransposeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 300);

    // === Guitar String Selector ===
    stringSelector.addItem("E (low)", 1);
    stringSelector.addItem("A", 2);
    stringSelector.addItem("D", 3);
    stringSelector.addItem("G", 4);
    stringSelector.addItem("B", 5);
    stringSelector.addItem("E (high)", 6);

    stringSelector.setSelectedId(static_cast<int>(audioProcessor.getSelectedString()) + 1);
    stringSelector.addListener(this);
    addAndMakeVisible(stringSelector);

    // === Transpose Slider ===
    transposeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    transposeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    transposeSlider.setRange(-12, 12, 1); // Typical ±1 octave
    transposeSlider.setValue(audioProcessor.getSemitoneShift()); // Use getter from processor
    transposeSlider.addListener(this);
    addAndMakeVisible(transposeSlider);
}

SingleStringTransposeAudioProcessorEditor::~SingleStringTransposeAudioProcessorEditor()
{
}

void SingleStringTransposeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Select a Guitar String:", 0, 30, getWidth(), 20, juce::Justification::centred, 1);
    g.drawFittedText ("Transpose (semitones):", 0, 180, getWidth(), 20, juce::Justification::centred, 1);
}

void SingleStringTransposeAudioProcessorEditor::resized()
{
    stringSelector.setBounds (getWidth() / 2 - 100, 60, 200, 30);
    transposeSlider.setBounds (getWidth() / 2 - 50, 200, 100, 100);
}

void SingleStringTransposeAudioProcessorEditor::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &stringSelector)
    {
        int selectedId = box->getSelectedId();
        audioProcessor.setSelectedString(static_cast<GuitarString>(selectedId - 1));
    }
}

void SingleStringTransposeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &transposeSlider)
    {
        audioProcessor.setSemitoneShift((int)slider->getValue());
    }
}
