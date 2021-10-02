#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
    
}

bool SoundSystem::Initialize(const char* filePath)
{
    system = NULL;
    FMOD_RESULT result;

    result = FMOD::Studio::System::create(&system);
    if (result != FMOD_OK)
    {
        return false;
    }
    result = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        return false;
    }


    result = FMOD::System_Create(&mpSystem);
    if (result != FMOD_OK)
    {
        return false;
    }
    result = mpSystem->init(512,FMOD_INIT_NORMAL,0 );
    if (result != FMOD_OK)
    {
        return false;
    }

    
   
    result = mpSystem->createSound(filePath, FMOD_DEFAULT, nullptr, &sound);

    if (result != FMOD_OK)
    {
        return false;
    }
   
 
    return true;
}

bool SoundSystem::Play()
{
    FMOD_RESULT result;
    result = mpSystem->playSound(sound, nullptr, false, &channel);
    if (result != FMOD_OK)
    {
        return false;
    }
    return true;
}

void SoundSystem::Update()
{
    mpSystem->update();
}
