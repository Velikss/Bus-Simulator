#pragma once

#define MAX_AUDIO_CHANNELS 32

#include <pch.hpp>
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <util/Formatter.hpp>
#include <vulkan/scene/Scene.hpp>

class cAudioHandler
{
private:
    typedef FMOD::Sound cSound;
    typedef FMOD::Channel cChannel;

    typedef std::map<string, cSound*> SoundMap;
    typedef std::map<uint, cChannel*> ChannelMap;
    typedef std::map<uint, glm::vec3> PositionMap;

    // FMOD instances
    FMOD::Studio::System* ppStudioSystem;
    FMOD::System* ppSystem;

    // Current active scene
    cScene* ppScene = nullptr;

    // Unique ID for the next channel
    uint puiNextChannelID = 0;

    // All the loaded sounds
    SoundMap pmSounds;
    // All the active (playing) channels
    ChannelMap pmChannels;
    // Positions of the active channels
    PositionMap pmPositions;

public:
    cAudioHandler();
    ~cAudioHandler();

    // Must be called in a main loop. Fixed update
    // rate is not required
    void Update();

    // Set the current active scene. Is used to position
    // the sounds relative to the camera
    void SetScene(cScene* pScene);

    // Load a sound with optional parameters
    void LoadSound(const string& sName, bool b3D = true, bool bLooping = false, bool bStream = false);
    // Unload a sound. Note that all sounds will be automatically
    // unloaded when the AudioHandler is destroyed
    void UnloadSound(const string& sName);

    // Play a sound at a specified position and volume
    // Returns the ID of the channel where this sound is playing
    // If the sound is not loaded, it will be loaded automatically
    // with default parameters
    uint PlaySound(const string& sName, glm::vec3& tPosition, float fVolume);
    // Check if a channel is currently playing
    bool IsPlaying(uint uiChannelId);

    // Set the position for a channel
    void SetChannelPosition(uint uiChannelId, glm::vec3& tPosition);
    // Set the volume for a channel
    void SetChannelVolume(uint uiChannelId, float fVolume);

private:
    // Transform a vector with the camera view matrix and
    // convert it to an FMOD vector
    FMOD_VECTOR TransformToFMODVec(glm::vec3& tVector);
};

cAudioHandler::cAudioHandler()
{
    // Initialize FMOD
    FMOD::Studio::System::create(&ppStudioSystem);
    ppStudioSystem->initialize(MAX_AUDIO_CHANNELS, FMOD_STUDIO_INIT_LIVEUPDATE,
                               FMOD_INIT_PROFILE_ENABLE, nullptr);
    ppStudioSystem->getCoreSystem(&ppSystem);
}

cAudioHandler::~cAudioHandler()
{
    // Unload all sounds and release the FMOD instance
    ppStudioSystem->unloadAll();
    ppStudioSystem->release();
}

void cAudioHandler::SetScene(cScene* pScene)
{
    assert(pScene != nullptr);

    ppScene = pScene;
}

void cAudioHandler::Update()
{
    std::vector<uint> aStoppedChannels;

    // Loop over all active channels
    for (auto& oChannel : pmChannels)
    {
        cChannel* pChannel = oChannel.second;

        bool bIsPlaying = false;
        pChannel->isPlaying(&bIsPlaying);
        // If the channel is no longer playing, add it to the list of stopped channels
        if (!bIsPlaying)
        {
            aStoppedChannels.push_back(oChannel.first);
        }
        else
        {
            // Get the mode from the sound
            FMOD_MODE eCurrMode;
            pChannel->getMode(&eCurrMode);

            // If 3D is enabled, update the channel position
            if (eCurrMode & FMOD_3D)
            {
                FMOD_VECTOR tFMODPos = TransformToFMODVec(pmPositions[oChannel.first]);
                pChannel->set3DAttributes(&tFMODPos, nullptr);
            }
        }
    }

    // Remove all stopped channels
    for (uint uiChannelId : aStoppedChannels)
    {
        pmChannels.erase(uiChannelId);
    }

    // Call the audio system update
    ppStudioSystem->update();
}

void cAudioHandler::LoadSound(const string& sName, bool b3D, bool bLooping, bool bStream)
{
    assert(sName.length() > 0);

    // Check if the sound is already loaded, if so, just return
    auto tResult = pmSounds.find(sName);
    if (tResult != pmSounds.end()) return;

    // Set the required bits in the mode based on the parameters
    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3D ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATESAMPLE;

    // Create the sound
    cSound* pSound = nullptr;
    ppSystem->createSound(sName.c_str(), eMode, nullptr, &pSound);

    // If the sound can't be created, throw an error
    if (pSound == nullptr)
    {
        throw std::runtime_error("Unable to load sound " + sName);
    }

    // Store the sound in our sounds map
    pmSounds[sName] = pSound;
}

void cAudioHandler::UnloadSound(const string& sName)
{
    assert(sName.length() > 0);

    // Check if the sound is not loaded, if so, just return
    auto tResult = pmSounds.find(sName);
    if (tResult == pmSounds.end()) return;

    // Release the sound and erase it from our sounds map
    tResult->second->release();
    pmSounds.erase(tResult);
}

uint cAudioHandler::PlaySound(const string& sName, glm::vec3& tPosition, float fVolume)
{
    assert(sName.length() > 0);
    assert(fVolume >= 0 && fVolume <= 1);           // Volume must be between 0 and 1
    assert(pmChannels.size() < MAX_AUDIO_CHANNELS); // Make sure we're within our channel limit

    uint uiChannelId = puiNextChannelID++;

    // Find the sound, and if it's not loaded, load it
    auto tResult = pmSounds.find(sName);
    if (tResult == pmSounds.end())
    {
        LoadSound(sName);
        tResult = pmSounds.find(sName);
    }
    cSound* pSound = tResult->second;

    // Initialize the sound on a channel but keep it paused for now
    cChannel* pChannel = nullptr;
    ppSystem->playSound(pSound, nullptr, true, &pChannel);

    // If the channel doesn't exist, throw an error
    if (pChannel == nullptr)
    {
        throw std::runtime_error("Unable to play sound " + sName);
    }

    // Get the mode from the sound
    FMOD_MODE eCurrMode;
    pSound->getMode(&eCurrMode);

    // If 3D is enabled, set the channel position
    if (eCurrMode & FMOD_3D)
    {
        pmPositions[uiChannelId] = tPosition;
        FMOD_VECTOR tFMODPos = TransformToFMODVec(tPosition);
        pChannel->set3DAttributes(&tFMODPos, nullptr);
    }

    // Set the channel volume and unpause it
    pChannel->setVolume(fVolume);
    pChannel->setPaused(false);

    // Store the channel to our channels map
    pmChannels[uiChannelId] = pChannel;

    return uiChannelId;
}

bool cAudioHandler::IsPlaying(uint uiChannelId)
{
    // Return true if a channel with that ID can be found
    return pmChannels.find(uiChannelId) != pmChannels.end();
}

void cAudioHandler::SetChannelPosition(uint uiChannelId, glm::vec3& tPosition)
{
    // Find the channel with the given ID, throw an error if none found
    auto tResult = pmChannels.find(uiChannelId);
    if (tResult == pmChannels.end())
    {
        throw std::runtime_error(cFormatter() << "Unable to find audio channel " << uiChannelId);
    }

    // Set the channel position
    pmPositions[uiChannelId] = tPosition;
    FMOD_VECTOR tFMODPos = TransformToFMODVec(tPosition);
    tResult->second->set3DAttributes(&tFMODPos, nullptr);
}

void cAudioHandler::SetChannelVolume(uint uiChannelId, float fVolume)
{
    assert(fVolume >= 0 && fVolume <= 1); // Volume must be between 0 and 1

    // Find the channel with the given ID, throw an error if none found
    auto tResult = pmChannels.find(uiChannelId);
    if (tResult == pmChannels.end())
    {
        throw std::runtime_error(cFormatter() << "Unable to find audio channel " << uiChannelId);
    }

    // Set the channel volume
    tResult->second->setVolume(fVolume);
}

FMOD_VECTOR cAudioHandler::TransformToFMODVec(glm::vec3& tVector)
{
    assert(ppScene != nullptr);

    // Transform the vector with the camera view pos to get a vector relative to the camera
    glm::vec3 tRelative = ppScene->GetCamera().GetViewMatrix() * glm::vec4(tVector, 1);

    return {tRelative.x, tRelative.y, tRelative.z};
}
