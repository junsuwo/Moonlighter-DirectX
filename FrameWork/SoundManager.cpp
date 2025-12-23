#include "SoundManager.h"
#include <iostream>
#include <fmod_errors.h>


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cerr << "FMOD Error: " << FMOD_ErrorString(result) << std::endl;
        exit(-1);
    }
}

SoundManager::SoundManager()
    : m_system(nullptr), m_bgmChannel(nullptr)
{
}

SoundManager::~SoundManager()
{
    Release();
}

bool SoundManager::Init()
{
    FMOD_RESULT result = FMOD::System_Create(&m_system);
    if (result != FMOD_OK) return false;

    result = m_system->init(512, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) return false;

    return true;
}

bool SoundManager::LoadBGM(const std::string& key, const std::string& filepath)
{
    if (!m_system) return false;

    // 기존에 로드된 사운드 있으면 해제
    auto it = m_bgmMap.find(key);
    if (it != m_bgmMap.end())
    {
        it->second->release();
        m_bgmMap.erase(it);
    }

    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = m_system->createSound(filepath.c_str(), FMOD_LOOP_NORMAL | FMOD_2D, nullptr, &sound);
    if (result != FMOD_OK) return false;

    m_bgmMap[key] = sound;
    return true;
}

void SoundManager::PlayBGM(const std::string& key)
{
    if (!m_system) return;

    // 이미 같은 BGM이 재생 중이면 재생 안 함
    if (m_bgmChannel)
    {
        bool isPlaying = false;
        m_bgmChannel->isPlaying(&isPlaying);

        if (isPlaying && m_currentBGMKey == key)
            return;  // 같은 곡이면 재생하지 않음

        // 다른 곡이면 정지
        m_bgmChannel->stop();
    }

    auto it = m_bgmMap.find(key);
    if (it == m_bgmMap.end())
    {
        std::cerr << "BGM key not found: " << key << std::endl;
        return;
    }

    FMOD_RESULT result = m_system->playSound(it->second, nullptr, false, &m_bgmChannel);
    if (result != FMOD_OK)
    {
        std::cerr << "Failed to play BGM: " << key << std::endl;
        return;
    }

    if (m_bgmChannel)
    {
        if (key == "dungeon")
            m_bgmChannel->setVolume(0.5f);
        else if (key == "boss")
            m_bgmChannel->setVolume(0.5f);
        else
            m_bgmChannel->setVolume(1.0f);
    }
    else
    {
        std::cerr << "Warning: m_bgmChannel is nullptr after playSound for key: " << key << std::endl;
    }

    m_currentBGMKey = key;  // 현재 재생중인 BGM 키 저장
}
void SoundManager::StopBGM()
{
    if (m_bgmChannel)
    {
        m_bgmChannel->stop();
        m_bgmChannel = nullptr;
    }
}

void SoundManager::Update()
{
    if (m_system)
    {
        m_system->update();
    }
}

void SoundManager::Release()
{
    for (auto& pair : m_bgmMap)
    {
        if (pair.second)
        {
            pair.second->release();
        }
    }
    m_bgmMap.clear();

    if (m_system)
    {
        m_system->close();
        m_system->release();
        m_system = nullptr;
    }
}
bool SoundManager::LoadSFX(const std::string& key, const std::string& filepath)
{
    if (!m_system) return false;

    auto it = m_sfxMap.find(key);
    if (it != m_sfxMap.end())
    {
        it->second->release();
        m_sfxMap.erase(it);
    }

    FMOD::Sound* sound = nullptr;
    FMOD_RESULT result = m_system->createSound(filepath.c_str(), FMOD_DEFAULT | FMOD_2D, nullptr, &sound);
    if (result != FMOD_OK) return false;

    m_sfxMap[key] = sound;
    return true;
}

void SoundManager::PlaySFX(const std::string& key)
{
    if (!m_system) return;

    auto it = m_sfxMap.find(key);
    if (it == m_sfxMap.end())
    {
        std::cerr << "SFX key not found: " << key << std::endl;
        return;
    }

    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = m_system->playSound(it->second, nullptr, false, &channel);
    if (result != FMOD_OK)
    {
        std::cerr << "Failed to play SFX: " << key << std::endl;
        return;
    }
    // 볼륨 조절
    channel->setVolume(1.0f);



    // 필요시 여러 채널 관리를 위해 벡터에 저장하거나,  
    // 나중에 채널 관리 추가 가능
}
