#pragma once

#include <JuceHeader.h>

//==============================================================================

/*
    This enum represents which string you want to process.
    You can change `selectedString` to any of these.
*/
enum class GuitarString
{
    E_low = 0,
    A     = 1,
    D     = 2,
    G     = 3,
    B     = 4,
    E_high = 5
};

//==============================================================================

class SingleStringTransposeAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SingleStringTransposeAudioProcessor();
    ~SingleStringTransposeAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // === Getters and setters for selectedString ===
    void setSelectedString(GuitarString str) { selectedString = str; }
    GuitarString getSelectedString() const { return selectedString; }

    // Semitone shifter
    void setSemitoneShift(int semitones) { semitoneShift = semitones; }
    int getSemitoneShift() const { return semitoneShift; }

private:
    //==============================================================================

    GuitarString selectedString = GuitarString::E_low;
    int semitoneShift = 0;

    // === DSP pitch shifter ===
 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SingleStringTransposeAudioProcessor)
};
