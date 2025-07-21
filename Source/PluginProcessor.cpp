#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath> // for std::pow

#include "../../rubberband/rubberband/RubberBandStretcher.h"
#include "../../rubberband/src/finer/R3Stretcher.h"

using namespace RubberBand; // Only in CPP, not in headers

//==============================================================================
SingleStringTransposeAudioProcessor::SingleStringTransposeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    selectedString = GuitarString::D; // Default string
    semitoneShift = 0;                // Default transposition

    // Example initialization (you can move this to prepareToPlay if needed)
    // stretcher = std::make_unique<RubberBandStretcher>(
    //     44100, 2,
    //     RubberBandStretcher::OptionProcessRealTime |
    //     RubberBandStretcher::OptionPitchHighQuality,
    //     1.0, 1.0
    // );
}

SingleStringTransposeAudioProcessor::~SingleStringTransposeAudioProcessor() {}

//==============================================================================
const juce::String SingleStringTransposeAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool SingleStringTransposeAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool SingleStringTransposeAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool SingleStringTransposeAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double SingleStringTransposeAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int SingleStringTransposeAudioProcessor::getNumPrograms() {
    return 1;
}

int SingleStringTransposeAudioProcessor::getCurrentProgram() {
    return 0;
}

void SingleStringTransposeAudioProcessor::setCurrentProgram(int) {}
const juce::String SingleStringTransposeAudioProcessor::getProgramName(int) { return {}; }
void SingleStringTransposeAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================
void SingleStringTransposeAudioProcessor::prepareToPlay(double, int) {
    // Optionally move RubberBandStretcher construction here
}

void SingleStringTransposeAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SingleStringTransposeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void SingleStringTransposeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    int stringChannel = static_cast<int>(selectedString);

    // TODO: Apply Rubber Band pitch shifting here
    // if (stretcher)
    // {
    //     float* channelData = buffer.getWritePointer(stringChannel);
    //     stretcher->process(&channelData, buffer.getNumSamples(), false);
    // }
}

//==============================================================================
bool SingleStringTransposeAudioProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* SingleStringTransposeAudioProcessor::createEditor() {
    return new SingleStringTransposeAudioProcessorEditor(*this);
}

//==============================================================================
void SingleStringTransposeAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    juce::MemoryOutputStream stream(destData, true);
    stream.writeInt(static_cast<int>(selectedString));
    stream.writeInt(semitoneShift); // Save transpose
}

void SingleStringTransposeAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    int storedId = stream.readInt();
    int storedShift = stream.readInt(); // Load transpose

    if (storedId >= 0 && storedId <= 5)
        selectedString = static_cast<GuitarString>(storedId);

    if (storedShift >= -12 && storedShift <= 12)
        semitoneShift = storedShift;
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new SingleStringTransposeAudioProcessor();
}
