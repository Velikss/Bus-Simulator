#pragma once

#define MAX_AUDIO_CHANNELS 64

#include <pch.hpp>
#include <fmod.hpp>
#include <util/Formatter.hpp>
#include <vulkan/scene/Camera.hpp>

class cAudioHandler
{
private:
    typedef FMOD::Sound cSound;
    typedef FMOD::Channel cChannel;

    typedef std::map<string, cSound*> SoundMap;
    typedef std::map<uint, cChannel*> ChannelMap;
    typedef std::map<uint, glm::vec3> PositionMap;

    // FMOD instances
    FMOD::System* ppSystem;

    // Current active scene
    Camera** ppCamera = nullptr;

    // Unique ID for the next channel
    uint puiNextChannelID = 0;

    // All the loaded sounds
    SoundMap pmSounds;
    // All the active (playing) channels
    ChannelMap pmChannels;
    // Positions of the active channels
    PositionMap pmPositions;

    const FMOD_VECTOR ptCameraUp = {0, 1, 0};
    FMOD_VECTOR ptCameraForward = {0, 0, 1};
    FMOD_VECTOR ptCameraPos = {0, 0, 0};

public:
    cAudioHandler();
    ~cAudioHandler();

    // Must be called every frame
    void Update();

    // Set the current active scene. Is used to position
    // the sounds relative to the camera
    void SetCamera(Camera** pCamera);

    // Load a sound with optional parameters
    void LoadSound(const string& sName, bool b3D = true, bool bLooping = false, bool bStream = false);
    // Unload a sound. Note that all sounds will be automatically
    // unloaded when the AudioHandler is destroyed
    void UnloadSound(const string& sName);

    // Play a sound at a specified position and volume
    // Returns the ID of the channel where this sound is playing
    // If the sound is not loaded, it will be loaded automatically
    // with default parameters
    uint PlaySound(const string& sName, glm::vec3 tPosition, float fVolume);

    // Check if a channel is currently playing
    bool IsPlaying(uint uiChannelId);
    // Pause or resume playback of a channel
    void SetPaused(uint uiChannelId, bool bPaused);

    // Set the position for a channel
    void SetChannelPosition(uint uiChannelId, glm::vec3 tPosition);
    // Set the volume for a channel
    void SetChannelVolume(uint uiChannelId, float fVolume);

private:
    // Translate a GLM vec3 into an FMOD_VECTOR
    FMOD_VECTOR GLMToFMODVec(glm::vec3 tVector);
};

cAudioHandler::cAudioHandler()
{
    // Initialize the FMOD instance
    FMOD::System_Create(&ppSystem);
    ppSystem->init(MAX_AUDIO_CHANNELS, FMOD_INIT_NORMAL, nullptr);
    ppSystem->set3DSettings(1.0, 2, 1.0);
}

cAudioHandler::~cAudioHandler()
{
    // Unload all sounds
    for (auto& oSound : pmSounds)
    {
        oSound.second->release();
    }

    // Release the FMOD instance
    ppSystem->release();
}

void cAudioHandler::SetCamera(Camera **pCamera)
{
    assert(pCamera != nullptr);

    ppCamera = pCamera;
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
        FMOD_MODE eCurrMode;
        pChannel->getMode(&eCurrMode);
        // If the channel is no longer playing, add it to the list of stopped channels
        if (!bIsPlaying && !(eCurrMode & FMOD_LOOP_NORMAL))
        {
            aStoppedChannels.push_back(oChannel.first);
        }
    }

    // Remove all stopped channels
    for (uint uiChannelId : aStoppedChannels)
    {
        pmChannels.erase(uiChannelId);
    }

    // Update the listener position
    ptCameraPos = GLMToFMODVec((*ppCamera)->cameraPos);

    // Update the listener direction
    glm::vec3 tDirection = glm::normalize((*ppCamera)->cameraFront * glm::vec3(-1, 0, -1));
    ptCameraForward = GLMToFMODVec(tDirection);

    // Update the 3D listener attributes
    ppSystem->set3DListenerAttributes(0, &ptCameraPos, nullptr, &ptCameraForward, &ptCameraUp);

    // Call the audio system update
    ppSystem->update();
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

uint cAudioHandler::PlaySound(const string& sName, glm::vec3 tPosition, float fVolume)
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
        FMOD_VECTOR tFMODPos = GLMToFMODVec(tPosition);
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

void cAudioHandler::SetPaused(uint uiChannelId, bool bPaused)
{
    // Find the channel with the given ID, throw an error if none found
    auto tResult = pmChannels.find(uiChannelId);
    if (tResult == pmChannels.end())
    {
        throw std::runtime_error(cFormatter() << "Unable to find audio channel " << uiChannelId);
    }

    // Set the paused value of the channel
    tResult->second->setPaused(bPaused);
}

void cAudioHandler::SetChannelPosition(uint uiChannelId, glm::vec3 tPosition)
{
    // Find the channel with the given ID, throw an error if none found
    auto tResult = pmChannels.find(uiChannelId);
    if (tResult == pmChannels.end())
    {
        throw std::runtime_error(cFormatter() << "Unable to find audio channel " << uiChannelId);
    }

    // Set the channel position
    pmPositions[uiChannelId] = tPosition;
    FMOD_VECTOR tFMODPos = GLMToFMODVec(tPosition);
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

FMOD_VECTOR cAudioHandler::GLMToFMODVec(glm::vec3 tVector)
{
    return {tVector.x, tVector.y, tVector.z};
}
