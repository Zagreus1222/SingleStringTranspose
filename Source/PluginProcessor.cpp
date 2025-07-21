#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath> // for std::pow

#include "../../rubberband/rubberband/RubberBandStretcher.h"
#include "../../rubberband/src/finer/R3Stretcher.h"

using namespace RubberBand;

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
    selectedString = GuitarString::D;
    semitoneShift = 0;
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

int SingleStringTransposeAudioProcessor::getNumPrograms() { return 1; }
int SingleStringTransposeAudioProcessor::getCurrentProgram() { return 0; }
void SingleStringTransposeAudioProcessor::setCurrentProgram(int) {}
const juce::String SingleStringTransposeAudioProcessor::getProgramName(int) { return {}; }
void SingleStringTransposeAudioProcessor::changeProgramName(int, const juce::String&) {}

//==============================================================================

void SingleStringTransposeAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    // Calculate pitch shift ratio
    float pitchRatio = std::pow(2.0f, semitoneShift / 12.0f);

    // Reset before re-initializing
    stretcher.reset();

    // Create a new RubberBandStretcher instance
    stretcher = std::make_unique<RubberBand::RubberBandStretcher>(
        sampleRate,
        1, // Only processing one channel (one string)
        RubberBand::RubberBandStretcher::OptionProcessRealTime |
        RubberBand::RubberBandStretcher::OptionPitchHighQuality |
        RubberBand::RubberBandStretcher::OptionThreadingAuto
    );

    // Set pitch shift (1.0 = no pitch change, >1 = up, <1 = down)
    stretcher->setPitchScale(pitchRatio);

    // Optional: make sure time stretching isn't used
    stretcher->setTimeRatio(1.0);

    // Optional: flush internal buffers
    stretcher->reset();
}



void SingleStringTransposeAudioProcessor::releaseResources() {
    stretcher.reset();
}

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

void SingleStringTransposeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    int stringChannel = static_cast<int>(selectedString);
    if (stringChannel >= numChannels || stretcher == nullptr)
        return;

    // Set the pitch ratio based on semitone parameter
    double ratio = std::pow(2.0, semitoneShift / 12.0);
    stretcher->setPitchScale(ratio);

    // Get input pointer
    float* input = buffer.getWritePointer(stringChannel);

    // Prepare RubberBand input
    std::vector<float*> inputPtrs = { input };
    stretcher->process(inputPtrs.data(), numSamples, false);

    // Retrieve output if available
    if (stretcher->available() > 0)
    {
        const int avail = std::min(stretcher->available(), numSamples);

        // Create a temporary buffer for output
        juce::AudioBuffer<float> tempBuffer(1, avail);
        float* output = tempBuffer.getWritePointer(0);
        std::vector<float*> outputPtrs = { output };

        stretcher->retrieve(outputPtrs.data(), avail);

        // Overwrite selected channel with pitch-shifted data
        buffer.copyFrom(stringChannel, 0, tempBuffer, 0, 0, avail);
    }

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
    stream.writeInt(semitoneShift);
}

void SingleStringTransposeAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    int storedId = stream.readInt();
    int storedShift = stream.readInt();

    if (storedId >= 0 && storedId <= 5)
        selectedString = static_cast<GuitarString>(storedId);

    if (storedShift >= -12 && storedShift <= 12)
        semitoneShift = storedShift;
}

//==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new SingleStringTransposeAudioProcessor();
}
