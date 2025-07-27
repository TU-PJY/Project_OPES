#include "QuadPent_Sound.h"

QP::Sound::~Sound() {
	Unload();
}

void QP::Sound::Load(const std::string& Directory, FMOD_MODE Option) {
	if (Loaded || !LoadFile(Directory, Option))
		return;

	Data->getLength(&TotalTime, FMOD_TIMEUNIT_MS);
	SoundSystem.System->createChannelGroup(nullptr, &ChannelGroup);
	ChannelGroup->setVolume(1.0);
	//ChannelGroup->set3DMinMaxDistance(20.0f, 300.0f);
	Loaded = true;
}

void QP::Sound::Unload() {
	if (!Loaded)
		return;

	Channel->stop();
	Channel = nullptr;

	Data->release();
}