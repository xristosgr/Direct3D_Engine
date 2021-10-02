#pragma once



#include <fmod\fmod_studio.hpp>
#include <fmod\fmod.hpp>
#include <DirectXMath.h>


class SoundSystem
{
public:

	SoundSystem();
	bool Initialize(const char* filePath);
	bool Play();
	void Update();

	DirectX::XMFLOAT3 pos;

	FMOD::Studio::System* system;
	FMOD::System* mpSystem;
	int mnNextChannelId;
	FMOD::Sound* sound = nullptr;
	FMOD::Channel* channel = nullptr;

};

