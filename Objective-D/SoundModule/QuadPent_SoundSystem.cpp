#include "QuadPent_Sound.h"

QP::QuadPent_SoundSystem QP::SoundSystem;

void QP::QuadPent_SoundSystem::Init() {
	if (InitState)
		return;

	Result = FMOD::System_Create(&System);

	if (Result != FMOD_OK)
		exit(EXIT_FAILURE);

	System->init(128, FMOD_INIT_NORMAL, ExtDvData);

	FMOD::ChannelGroup* MasterGroup;
	System->getMasterChannelGroup(&MasterGroup);

	float Volume;
	MasterGroup->getVolume(&Volume);
	if (Volume == 0.0f)
		MasterGroup->setVolume(1.0f);

	System->set3DSettings(1.0, 1.0, 2.0);

	InitState = true;
}

void QP::QuadPent_SoundSystem::SetSurrondValue(float DoplerScale, float DistanceFactor, float RolloffScale) {
	System->set3DSettings(DoplerScale, DistanceFactor, RolloffScale);
}

void QP::QuadPent_SoundSystem::SetListenerPosition(const xmfloat2& Position) {
	ListenerPosition.x = Position.x;
	ListenerPosition.y = Position.y;
	ListenerPosition.z = 0.0f;
	System->set3DListenerAttributes(0, &ListenerPosition, 0, 0, 0);
}

void QP::QuadPent_SoundSystem::Update() {
	System->update();
}

void QP::QuadPent_SoundSystem::StopAllSounds() {
	FMOD::ChannelGroup* MasterChannelGroup{};
	System->getMasterChannelGroup(&MasterChannelGroup);
	MasterChannelGroup->stop();
}