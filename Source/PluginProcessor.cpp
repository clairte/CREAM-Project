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
    synth.addVoice(new SynthVoice());
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

//==============================================================================
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
     
    //**: the values here need exact modification!! 这里的数值是按照我们做的synthsizer变的！
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
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*> (synth.getVoice(i)))
        {
            auto& oscWaveChoice = *apvts.getRawParameterValue("OSC1WAVETYPE");
            auto& fmDepth = *apvts.getRawParameterValue("OSC1FMFREQ");
            auto& fmFreq = *apvts.getRawParameterValue("OSC1FMDEPTH");
            
            //Amp Adsr
            auto& attack = *apvts.getRawParameterValue("ATTACK"); //returns std atomic pointer
            auto& decay = *apvts.getRawParameterValue("DECAY");
            auto& sustain = *apvts.getRawParameterValue("SUSTAIN");
            auto& release = *apvts.getRawParameterValue("RELEASE");
            
            //Filter
            auto& filterType = *apvts.getRawParameterValue("FILTERTYPE");
            auto& cutoff = *apvts.getRawParameterValue("FILTERCUTOFF");
            auto& resonance = *apvts.getRawParameterValue("FILTERRES");
            
            //Mod Adsr
            auto& modAttack = *apvts.getRawParameterValue("MODATTACK"); //returns std atomic pointer
            auto& modDecay = *apvts.getRawParameterValue("MODDECAY");
            auto& modSustain = *apvts.getRawParameterValue("MODSUSTAIN");
            auto& modRelease = *apvts.getRawParameterValue("MODRELEASE");
            
            voice->getOscillator().setWaveType(oscWaveChoice);
            voice->getOscillator().updateFm(fmDepth,fmFreq);
            voice->updateAdsr(attack.load(), decay.load(), sustain.load(), release.load());
            voice->updateFilter(filterType.load(), cutoff.load(), resonance.load());
            voice->updateModAdsr(modAttack, modDecay, modSustain, modRelease);
           
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
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



//needs modifications of real values!


// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CREAMProjectAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout CREAMProjectAudioProcessorEditor::createParams()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // OSC select
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("OSC1", "Oscillator 1", juce::StringArray { "Sine", "Saw", "Square" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("OSC2", "Oscillator 2", juce::StringArray { "Sine", "Saw", "Square" }, 0));
    
    // OSC Gain
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1GAIN", "Oscillator 1 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, 0.1f, "dB"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC2GAIN", "Oscillator 2 Gain", juce::NormalisableRange<float> { -40.0f, 0.2f, 0.1f }, 0.1f, "dB"));
    
    // OSC Pitch val
    params.push_back (std::make_unique<juce::AudioParameterInt>("OSC1PITCH", "Oscillator 1 Pitch", -48, 48, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt>("OSC2PITCH", "Oscillator 2 Pitch", -48, 48, 0));
    
    // FM Osc Freq
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1FMFREQ", "Oscillator 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.1f }, 0.0f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC2FMFREQ", "Oscillator 1 FM Frequency", juce::NormalisableRange<float> { 0.0f, 1000.0f, 0.1f }, 0.0f, "Hz"));
    
    // FM Osc Depth
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC1FMDEPTH", "Oscillator 1 FM Depth", juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("OSC2FMDEPTH", "Oscillator 2 FM Depth", juce::NormalisableRange<float> { 0.0f, 100.0f, 0.1f }, 0.0f, ""));
    
    // LFO
    params.push_back (std::make_unique<juce::AudioParameterFloat>("LFO1FREQ", "LFO1 Frequency", juce::NormalisableRange<float> { 0.0f, 20.0f, 0.1f }, 0.0f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("LFO1DEPTH", "LFO1 Depth", juce::NormalisableRange<float> { 0.0f, 10000.0f, 0.1f, 0.3f }, 0.0f, ""));
    
    //Filter
    params.push_back (std::make_unique<juce::AudioParameterChoice>("FILTERTYPE", "Filter Type", juce::StringArray { "Low Pass", "Band Pass", "High Pass" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERCUTOFF", "Filter Cutoff", juce::NormalisableRange<float> { 20.0f, 20000.0f, 0.1f, 0.6f }, 20000.0f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERRESONANCE", "Filter Resonance", juce::NormalisableRange<float> { 0.1f, 2.0f, 0.1f }, 0.1f, ""));
    
    // ADSR
    params.push_back (std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.1f, 1.0f, 0.1f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> { 0.1f, 3.0f, 0.1f }, 0.4f));
    
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERADSRDEPTH", "Filter ADSR Depth", juce::NormalisableRange<float> { 0.0f, 10000.0f, 0.1f, 0.3f }, 10000.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERATTACK", "Filter Attack", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.01f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERDECAY", "Filter Decay", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.1f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERSUSTAIN", "Filter Sustain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("FILTERRELEASE", "Filter Release", juce::NormalisableRange<float> { 0.0f, 3.0f, 0.1f }, 0.1f));
    
    // Reverb
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBSIZE", "Reverb Size", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBWIDTH", "Reverb Width", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDAMPING", "Reverb Damping", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.5f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBDRY", "Reverb Dry", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 1.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBWET", "Reverb Wet", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("REVERBFREEZE", "Reverb Freeze", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.1f }, 0.0f, ""));
    
    return { params.begin(), params.end() };
}

void TapSynthAudioProcessor::setParams()
{
    setVoiceParams();
    setFilterParams();
    setReverbParams();
}

void CREAMProjectAudioProcessorEditor::setVoiceParams()
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            auto& attack = *apvts.getRawParameterValue ("ATTACK");
            auto& decay = *apvts.getRawParameterValue ("DECAY");
            auto& sustain = *apvts.getRawParameterValue ("SUSTAIN");
            auto& release = *apvts.getRawParameterValue ("RELEASE");
            
            auto& osc1Choice = *apvts.getRawParameterValue ("OSC1");
            auto& osc2Choice = *apvts.getRawParameterValue ("OSC2");
            auto& osc1Gain = *apvts.getRawParameterValue ("OSC1GAIN");
            auto& osc2Gain = *apvts.getRawParameterValue ("OSC2GAIN");
            auto& osc1Pitch = *apvts.getRawParameterValue ("OSC1PITCH");
            auto& osc2Pitch = *apvts.getRawParameterValue ("OSC2PITCH");
            auto& osc1FmFreq = *apvts.getRawParameterValue ("OSC1FMFREQ");
            auto& osc2FmFreq = *apvts.getRawParameterValue ("OSC2FMFREQ");
            auto& osc1FmDepth = *apvts.getRawParameterValue ("OSC1FMDEPTH");
            auto& osc2FmDepth = *apvts.getRawParameterValue ("OSC2FMDEPTH");
            
            auto& filterAttack = *apvts.getRawParameterValue ("FILTERATTACK");
            auto& filterDecay = *apvts.getRawParameterValue ("FILTERDECAY");
            auto& filterSustain = *apvts.getRawParameterValue ("FILTERSUSTAIN");
            auto& filterRelease = *apvts.getRawParameterValue ("FILTERRELEASE");

            auto& osc1 = voice->getOscillator1();
            auto& osc2 = voice->getOscillator2();
            
            auto& adsr = voice->getAdsr();
            auto& filterAdsr = voice->getFilterAdsr();
           
            for (int i = 0; i < getTotalNumOutputChannels(); i++)
            {
                osc1[i].setParams (osc1Choice, osc1Gain, osc1Pitch, osc1FmFreq, osc1FmDepth);
                osc2[i].setParams (osc2Choice, osc2Gain, osc2Pitch, osc2FmFreq, osc2FmDepth);
            }
            
            adsr.update (attack.load(), decay.load(), sustain.load(), release.load());
            filterAdsr.update (filterAttack, filterDecay, filterSustain, filterRelease);
        }
    }
}

void CREAMProjectAudioProcessorEditor::setFilterParams()
{
    auto& filterType = *apvts.getRawParameterValue ("FILTERTYPE");
    auto& filterCutoff = *apvts.getRawParameterValue ("FILTERCUTOFF");
    auto& filterResonance = *apvts.getRawParameterValue ("FILTERRESONANCE");
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

void CREAMProjectAudioProcessorEditor::setReverbParams()
{
    reverbParams.roomSize = *apvts.getRawParameterValue ("REVERBSIZE");
    reverbParams.width = *apvts.getRawParameterValue ("REVERBWIDTH");
    reverbParams.damping = *apvts.getRawParameterValue ("REVERBDAMPING");
    reverbParams.dryLevel = *apvts.getRawParameterValue ("REVERBDRY");
    reverbParams.wetLevel = *apvts.getRawParameterValue ("REVERBWET");
    reverbParams.freezeMode = *apvts.getRawParameterValue ("REVERBFREEZE");
    
    reverb.setParameters (reverbParams);
}


