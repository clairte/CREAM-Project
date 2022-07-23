#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

CREAMProjectAudioProcessor::CREAMProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    ,apvts (*this, nullptr, ProjectInfo::projectName, Utility::ParameterHelper::createParameterLayout())
#endif
{
    synth.addSound(new SynthSound());
   
    for (int i = 0; i < 5; i++)
    {
        synth.addVoice (new SynthVoice());
    }
    
    apvts.state.setProperty(Service::PresetManager::presetNameProperty, "", nullptr);
    apvts.state.setProperty("version", ProjectInfo::versionString, nullptr);
    
    presetManager = std::make_unique<Service::PresetManager>(apvts); 
}


CREAMProjectAudioProcessor::~CREAMProjectAudioProcessor()
{
}


const juce::String CREAMProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CREAMProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CREAMProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CREAMProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CREAMProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CREAMProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CREAMProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CREAMProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CREAMProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void CREAMProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}



void CREAMProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        //if we casted synthesiser voice pointer as a synthvoice pointer
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
     
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    reverbParams.roomSize = 0.5f;
    reverbParams.width = 1.0f;
    reverbParams.damping = 0.5f;
    reverbParams.freezeMode = 0.0f;
    reverbParams.dryLevel = 1.0f;
    reverbParams.wetLevel = 0.0f;
    
    reverb.setParameters (reverbParams);
}

void CREAMProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CREAMProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CREAMProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    setParams();
    
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    //Process reverb 
    juce::dsp::AudioBlock<float> block { buffer };
    reverb.process (juce::dsp::ProcessContextReplacing<float> (block));
    
}

bool CREAMProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CREAMProjectAudioProcessor::createEditor()
{
    return new CREAMProjectAudioProcessorEditor (*this);
}

void CREAMProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();
    const auto xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CREAMProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState == nullptr) { return; }
    const auto newTree = juce::ValueTree::fromXml(*xmlState);
    apvts.replaceState(newTree);
}


// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CREAMProjectAudioProcessor();
}

void CREAMProjectAudioProcessor::setParams()
{
    setVoiceParams();
    setFilterParams();
    setReverbParams();
}

void CREAMProjectAudioProcessor::setVoiceParams()
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            auto& osc1 = voice->getOscillator1();
            auto& osc2 = voice->getOscillator2();
            
            auto& adsr = voice->getAdsr();
            auto& filterAdsr = voice->getFilterAdsr(); 
            
            auto& osc1Choice = *apvts.getRawParameterValue("OSC1TYPE");
            auto& osc2Choice = *apvts.getRawParameterValue("OSC2TYPE");
            auto& osc1Gain = *apvts.getRawParameterValue("OSC1GAIN");
            auto& osc2Gain = *apvts.getRawParameterValue("OSC2GAIN");
            auto& osc1Pitch = *apvts.getRawParameterValue("OSC1PITCH");
            auto& osc2Pitch = *apvts.getRawParameterValue("OSC2PITCH");
            auto& osc1FmFreq = *apvts.getRawParameterValue("OSC1FMFREQ");
            auto& osc2FmFreq = *apvts.getRawParameterValue("OSC2FMFREQ");
            auto& osc1FmDepth = *apvts.getRawParameterValue("OSC1FMDEPTH");
            auto& osc2FmDepth = *apvts.getRawParameterValue("OSC2FMDEPTH");

            //Amp Adsr
            auto& attack = *apvts.getRawParameterValue("ATTACK"); //returns std atomic pointer
            auto& decay = *apvts.getRawParameterValue("DECAY");
            auto& sustain = *apvts.getRawParameterValue("SUSTAIN");
            auto& release = *apvts.getRawParameterValue("RELEASE");
            

            //Filter Adsr
            auto& filterAttack = *apvts.getRawParameterValue("FILTERATTACK"); //returns std atomic pointer
            auto& filterDecay = *apvts.getRawParameterValue("FILTERDECAY");
            auto& filterSustain = *apvts.getRawParameterValue("FILTERSUSTAIN");
            auto& filterRelease = *apvts.getRawParameterValue("FILTERRELEASE");

            for (int i = 0; i < getTotalNumOutputChannels(); i++)
            {
                osc1[i].setParams(osc1Choice,osc1Gain, osc1Pitch, osc1FmFreq, osc1FmDepth);
                osc2[i].setParams(osc2Choice,osc2Gain, osc2Pitch, osc2FmFreq, osc2FmDepth);
            }
            
            adsr.updateADSR(attack.load(), decay.load(), sustain.load(), release.load());
            filterAdsr.updateADSR(filterAttack, filterDecay, filterSustain, filterRelease);
        }
    }
}


void CREAMProjectAudioProcessor::setFilterParams()
{
    auto& filterType = *apvts.getRawParameterValue("FILTERTYPE");
    auto& filterCutoff = *apvts.getRawParameterValue("FILTERCUTOFF");
    auto& filterResonance = *apvts.getRawParameterValue("FILTERRES");
    auto& adsrDepth = *apvts.getRawParameterValue ("FILTERADSRDEPTH");
    auto& lfoFreq = *apvts.getRawParameterValue ("LFO1FREQ");
    auto& lfoDepth = *apvts.getRawParameterValue ("LFO1DEPTH");

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->updateModParams (filterType, filterCutoff, filterResonance, adsrDepth, lfoFreq, lfoDepth);
        }
    }
}


void CREAMProjectAudioProcessor::setReverbParams()
{
    reverbParams.roomSize = *apvts.getRawParameterValue ("REVERBSIZE");
    reverbParams.width = *apvts.getRawParameterValue ("REVERBWIDTH");
    reverbParams.damping = *apvts.getRawParameterValue ("REVERBDAMPING");
    reverbParams.dryLevel = *apvts.getRawParameterValue ("REVERBDRY");
    reverbParams.wetLevel = *apvts.getRawParameterValue ("REVERBWET");
    reverbParams.freezeMode = *apvts.getRawParameterValue ("REVERBFREEZE");

    reverb.setParameters (reverbParams);
}


