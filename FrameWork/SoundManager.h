#pragma once
#include <map>  // <unordered_map> 대신
#include <string>
#include <fmod.hpp>
#include <vector>



class SoundManager
{
public:
    SoundManager();
    ~SoundManager();

    bool Init();
    bool LoadBGM(const std::string& key, const std::string& filepath);
    void PlayBGM(const std::string& key);
    void StopBGM();

    bool LoadSFX(const std::string& key, const std::string& filepath);
    void PlaySFX(const std::string& key);


    void Update();
    void Release();


  


private:


    FMOD::System* m_system = nullptr;

    std::map<std::string, FMOD::Sound*> m_bgmMap;
    std::map<std::string, FMOD::Sound*> m_sfxMap;  // 효과음 저장용

    FMOD::Channel* m_bgmChannel = nullptr;
    std::vector<FMOD::Channel*> m_sfxChannels;

    std::string m_currentBGMKey;

};