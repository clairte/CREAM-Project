#include "PresetManager.h"

namespace Service
{
    const juce::File PresetManager::defaultDirectory
    {
        juce::File::getSpecialLocation(juce::File::SpecialLocationType::commonDocumentsDirectory)
            .getChildFile(ProjectInfo::companyName) //try to form valid uri
            .getChildFile(ProjectInfo::projectName)
    };

    const juce::String PresetManager::extension {"preset"};

    const juce::String PresetManager::presetNameProperty { "presetName"};

    PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts) :
        valueTreeStatePreset(apvts)
    {
        //Create a default Preset Directory, if it does not exist
        if(!defaultDirectory.exists())
        {
            const auto result = defaultDirectory.createDirectory(); //returns a result object
            if (result.failed())
            {
                DBG("Could not create preset directory: " + result.getErrorMessage());
                jassertfalse;
            }
        }
        
        //query preset name property that might exist in state
        
        /*
         returns a Value object
         refersTo;
         currentPreset as a value object will be in affect with property within apvts state,
         connection breaks whenever you replace the value tree object with another one
         (because you are replacing the state of the value tree state)
         */
        
        apvts.state.addListener(this); //when we notice state has been replcaed-> reestablish connection
        currentPreset.referTo(apvts.state.getPropertyAsValue(presetNameProperty, nullptr));
        
        
    }

    void PresetManager::savePreset(const juce::String& presetName)
    {
        if (presetName.isEmpty()) { return;}
        
        currentPreset.setValue(presetName); //set name before you write to the file
        const auto xml = valueTreeStatePreset.copyState().createXml(); //return unique pointer of an xml element
        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (!xml->writeTo(presetFile))
        {
            DBG("Could not create preset file: " + presetFile.getFullPathName());
            jassertfalse;
        }
        
    }

    void PresetManager::deletePreset(const juce::String& presetName)
    {
        if (presetName.isEmpty()) { return; }
        
        //check if preset file exists
        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (!presetFile.existsAsFile())
        {
            DBG("Preset file" + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }
        
        if (!presetFile.deleteFile()) //returns a bool
        {
            DBG("Preset file" + presetFile.getFullPathName() + "could not be deleted");
            jassertfalse;
            return;
        }
        
        //reset current preset to nothing
        currentPreset.setValue("");
    }

    void PresetManager::loadPreset(const juce::String& presetName)
    {
        if (presetName.isEmpty()) { return; }
        
        const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
        if (!presetFile.existsAsFile())
        {
            DBG("Preset file" + presetFile.getFullPathName() + " does not exist");
            jassertfalse;
            return;
        }
        
        // presetFile (XML) convert contents into a value tree; replace state of our plugin with this new value tree
        
        juce::XmlDocument xmlDocument { presetFile }; //accepts either a string or file
        const auto valueTreeToLoad = juce::ValueTree::fromXml(*xmlDocument.getDocumentElement()); //returns root of document element for that xml

        valueTreeStatePreset.replaceState(valueTreeToLoad);
        
        currentPreset.setValue(presetName);
    }

    //load next and previous will require list of presets to cycle through
    int PresetManager::loadNextPreset()
    {
        const auto allPresets = getAllPresets();
        if (allPresets.isEmpty()) { return -1; }
        
        const auto currentIndex = allPresets.indexOf(currentPreset.toString());
        const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
        
        loadPreset(allPresets.getReference(nextIndex));
        
        return nextIndex;
    }

    int PresetManager::loadPreviousPreset()
    {
        const auto allPresets = getAllPresets();
        if (allPresets.isEmpty()) { return -1; }
        
        const auto currentIndex = allPresets.indexOf(currentPreset.toString());
        const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() -1 : currentIndex -1;
        
        loadPreset(allPresets.getReference(previousIndex));
        
        return previousIndex; 
    }

    juce::StringArray PresetManager::getAllPresets() const
    {
        juce::StringArray presets;
        
        //find all preset files within default directory
        const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
        for (const auto& file : fileArray)
        {
            presets.add(file.getFileNameWithoutExtension());
        }
        
        return presets;
    }

    juce::String PresetManager::getCurrentPreset() const
    {
        return currentPreset.toString();
    }

    void PresetManager::valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged)
    {
        //reestablish connection between currentPreset and state property
        currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
    }
}
